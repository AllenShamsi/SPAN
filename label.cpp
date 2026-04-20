#include "label.h"
#include "mainWindow.h"

#include <QMouseEvent>
#include <QPolygonF>
#include <QTimer>

#include <algorithm>
#include <limits>
#include <cmath>

Label::Label(QCustomPlot *plot)
    : QObject(plot), m_plot(plot)
{
    connect(m_plot, &QCustomPlot::mousePress, this, [this](QMouseEvent *event) {
        if (event->button() != Qt::RightButton)
            return;

        double clickedX;

        // If mainWindow set autoAnnotateX, use that directly.
        QVariant autoVar = m_plot->property("autoAnnotateX");
        if (autoVar.isValid()) {
            clickedX = autoVar.toDouble();
            m_plot->setProperty("autoAnnotateX", QVariant());
        } else {
            clickedX = m_plot->xAxis->pixelToCoord(event->pos().x());
        }

        if (event->modifiers() & Qt::ShiftModifier)
        {
            QString legendName;
            if (m_plot->graphCount() > 0 && !m_plot->graph(0)->name().isEmpty())
                legendName = m_plot->graph(0)->name();
            else {
                legendName = "Spectrogram";
                return;
            }

            const bool isSingleChannel = (countTrailingLowerCase(legendName) == 1);

            if (!isSingleChannel) {
                placeLabelAt(clickedX, QString());
                return;
            }

            // Snap to nearest extremum in this graph
            const double nearestX = findNearestPeak(clickedX, legendName);
            placeLabelAt(nearestX, QStringLiteral("MaxC"));

            if (auto *mw = qobject_cast<mainWindow*>(m_plot->window())) {
                std::vector<double> velocityData = mw->getPrecomputedVelocity(legendName);
                if (!velocityData.empty()) {
                    const double sr = mw->getCurrentSpanWavSR();

                    landmarks lm;
                    findPrecedingVelocityExtremumFromVector(lm, nearestX, velocityData, sr);
                    findFollowingVelocityExtremumFromVector (lm, nearestX, velocityData, sr);

                    if (std::isfinite(lm.GONS))  placeLabelAt(lm.GONS,  QStringLiteral("GONS"));
                    if (std::isfinite(lm.NONS))  placeLabelAt(lm.NONS,  QStringLiteral("NONS"));
                    if (std::isfinite(lm.PVEL1)) emit landmarkAdded(legendName, lm.PVEL1, QStringLiteral("PVEL1"));
                    if (std::isfinite(lm.PVEL2)) emit landmarkAdded(legendName, lm.PVEL2, QStringLiteral("PVEL2"));
                    if (std::isfinite(lm.NOFFS)) placeLabelAt(lm.NOFFS, QStringLiteral("NOFFS"));
                    if (std::isfinite(lm.GOFFS)) placeLabelAt(lm.GOFFS, QStringLiteral("GOFFS"));
                } else {
                    placeLabelAt(nearestX, QString());
                }
            }
            return;
        }

        // No Shift: just place a plain label at the clicked position
        placeLabelAt(clickedX, QString());
    });

    // Left-button drag handling
    connect(m_plot, &QCustomPlot::mousePress,   this, &Label::onMousePress);
    connect(m_plot, &QCustomPlot::mouseMove,    this, &Label::onMouseMove);
    connect(m_plot, &QCustomPlot::mouseRelease, this, &Label::onMouseRelease);

    connect(m_plot, &QCustomPlot::itemClick, this, &Label::onItemClicked);
    m_plot->setSelectionTolerance(1);

    m_plot->setMouseTracking(true);
    m_plot->show();
    m_plot->setInteraction(QCP::iRangeDrag, false);
    m_plot->setInteraction(QCP::iRangeZoom, false);
}

namespace {
inline bool plotHasItem(QCustomPlot* plot, QCPAbstractItem* it) {
    if (!plot || !it) return false;
    const int n = plot->itemCount();
    for (int i = 0; i < n; ++i)
        if (plot->item(i) == it) return true;
    return false;
}
inline void safeRemove(QCustomPlot* plot, QCPAbstractItem* it) {
    if (plotHasItem(plot, it)) plot->removeItem(it);
}
} // namespace


int Label::countTrailingLowerCase(const QString &str)
{
    int count = 0;
    for (int i = str.size() - 1; i >= 0; --i) {
        if (str[i].isLower()) ++count;
        else break;
    }
    return count;
}

bool Label::isHandle(const QCPItemText* item) const
{
    if (!item) return false;
    for (auto it = m_labelHandleMapText.constBegin(); it != m_labelHandleMapText.constEnd(); ++it) {
        if (static_cast<const QCPItemText*>(it.key()) == item)
            return true;
    }
    return false;
}

QCPItemLine* Label::lineForHandle(const QCPItemText* item) const
{
    if (!item) return nullptr;
    for (auto it = m_labelHandleMapText.constBegin(); it != m_labelHandleMapText.constEnd(); ++it) {
        if (static_cast<const QCPItemText*>(it.key()) == item)
            return it.value();
    }
    return nullptr;
}

double Label::findNearestPeak(double xClicked, const QString &labelName)
{
    if (!m_plot)
        return xClicked;

    // 1) Pick which graph we are working on
    QCPGraph *targetGraph = nullptr;
    for (int i = 0; i < m_plot->graphCount(); ++i) {
        if (m_plot->graph(i)->name() == labelName) {
            targetGraph = m_plot->graph(i);
            break;
        }
    }
    if (!targetGraph && m_plot->graphCount() > 0)
        targetGraph = m_plot->graph(0);
    if (!targetGraph)
        return xClicked;

    QSharedPointer<QCPGraphDataContainer> data = targetGraph->data();
    if (!data || data->size() < 3)
        return xClicked;

    const int N = data->size();
    auto itBegin = data->constBegin();
    auto itEnd   = data->constEnd();

    // --- 2. Decide what kind of extremum we "prefer" near xClicked ---

    enum class PeakType { Any, Max, Min };
    PeakType desired = PeakType::Any;

    int idx0 = -1;
    {
        double best = std::numeric_limits<double>::max();
        int idx = 0;
        for (auto it = itBegin; it != itEnd; ++it, ++idx) {
            double dist = std::fabs(it->key - xClicked);
            if (dist < best) {
                best = dist;
                idx0 = idx;
            }
        }
    }

    if (idx0 > 0 && idx0 < N - 1) {
        auto it0   = itBegin + idx0;
        double prevY = (it0 - 1)->value;
        double currY = it0->value;
        double nextY = (it0 + 1)->value;

        double slopeL = currY - prevY;
        double slopeR = nextY - currY;

        // Simple curvature test:
        if (slopeL >= 0.0 && slopeR <= 0.0) {
            // going up then down: hill-like
            desired = PeakType::Max;
        } else if (slopeL <= 0.0 && slopeR >= 0.0) {
            // going down then up: bowl-like
            desired = PeakType::Min;
        } else {
            // Fallback: check if current point is already a local extremum
            bool isLocalMax = (currY > prevY && currY >= nextY) ||
                              (currY >= prevY && currY > nextY);
            bool isLocalMin = (currY < prevY && currY <= nextY) ||
                              (currY <= prevY && currY < nextY);
            if (isLocalMax)      desired = PeakType::Max;
            else if (isLocalMin) desired = PeakType::Min;
            else                 desired = PeakType::Any;
        }
    }

    // --- 3. Scan all extrema, track nearest of each type ---

    // Limit snapping to a window around the click (25% of visible range)
    double maxSnapDist = std::numeric_limits<double>::max();
    {
        double rangeSize = m_plot->xAxis->range().size();
        if (rangeSize > 0.0)
            maxSnapDist = 0.25 * rangeSize;
    }

    bool   haveAny = false, haveMax = false, haveMin = false;
    double bestAnyX  = xClicked;
    double bestAnyD  = std::numeric_limits<double>::max();
    double bestMaxX  = xClicked;
    double bestMaxD  = std::numeric_limits<double>::max();
    double bestMinX  = xClicked;
    double bestMinD  = std::numeric_limits<double>::max();

    auto itLast = itEnd - 1;
    for (auto it = itBegin + 1; it != itLast; ++it) {
        double prevY = (it - 1)->value;
        double currY = it->value;
        double nextY = (it + 1)->value;

        bool isMax = (currY > prevY && currY > nextY);
        bool isMin = (currY < prevY && currY < nextY);
        if (!isMax && !isMin)
            continue;

        double px   = it->key;
        double dist = std::fabs(px - xClicked);

        // Don't snap to something very far away in time
        if (dist > maxSnapDist)
            continue;

        // Any extremum
        if (!haveAny || dist < bestAnyD) {
            haveAny = true;
            bestAnyX = px;
            bestAnyD = dist;
        }

        // Max-only
        if (isMax && (!haveMax || dist < bestMaxD)) {
            haveMax = true;
            bestMaxX = px;
            bestMaxD = dist;
        }

        // Min-only
        if (isMin && (!haveMin || dist < bestMinD)) {
            haveMin = true;
            bestMinX = px;
            bestMinD = dist;
        }
    }

    if (!haveAny)
        return xClicked;  // no extremum near click, don't move

    // --- 4. Choose extremum: prefer same "shape" if not much farther away ---

    const double factor = 1.5;  // how much farther we're willing to go for the right type

    if (desired == PeakType::Max && haveMax) {
        if (bestMaxD <= factor * bestAnyD)
            return bestMaxX;
    } else if (desired == PeakType::Min && haveMin) {
        if (bestMinD <= factor * bestAnyD)
            return bestMinX;
    }

    // Fallback: original behavior – nearest extremum of either type
    return bestAnyX;
}


void Label::findPrecedingVelocityExtremumFromVector(Label::landmarks &lm,
                                                    double xCenterSec,
                                                    const std::vector<double> &velocityData,
                                                    double sr)
{
    lm.MaxC = xCenterSec;

    if (!std::isfinite(xCenterSec))
        return;

    // Need at least 3 samples to detect a local extremum.
    if (velocityData.size() < 3)
        return;

    // 1) Find the sample index closest to xCenterSec.
    int nearestIndex = -1;
    double minDist   = std::numeric_limits<double>::max();
    for (int i = 0; i < static_cast<int>(velocityData.size()); ++i) {
        double tSec = i / sr;
        double dist = std::fabs(tSec - xCenterSec);
        if (dist < minDist) {
            minDist = dist;
            nearestIndex = i;
        }
    }
    if (nearestIndex < 1)
        return;

    // 2) Determine desired extremum type (closing peak) near MaxC.
    enum class ExtremumType { LocalMaximum, LocalMinimum };
    ExtremumType desired;
    if (velocityData[nearestIndex] > velocityData[nearestIndex - 1])
        desired = ExtremumType::LocalMinimum;
    else if (velocityData[nearestIndex] < velocityData[nearestIndex - 1])
        desired = ExtremumType::LocalMaximum;
    else {
        int j = nearestIndex;
        while (j > 0 && velocityData[j] == velocityData[j - 1])
            --j;
        if (j == 0)
            return;
        desired = (velocityData[nearestIndex] > velocityData[j - 1])
                      ? ExtremumType::LocalMinimum
                      : ExtremumType::LocalMaximum;
    }

    // 3) Scan left from nearestIndex to find PVEL1 (peak closing velocity).
    double y_PVEL1   = std::numeric_limits<double>::quiet_NaN();
    int    pvelIndex = -1;
    for (int i = nearestIndex - 1; i >= 1; --i) {
        if (i < 1 || i + 1 >= static_cast<int>(velocityData.size()))
            continue;

        double prevVal = velocityData[i - 1];
        double currVal = velocityData[i];
        double nextVal = velocityData[i + 1];

        if (!std::isfinite(currVal))
            continue;

        bool isLocalMaximum = (currVal > prevVal && currVal >= nextVal);
        bool isLocalMinimum = (currVal < prevVal && currVal <= nextVal);

        if (desired == ExtremumType::LocalMaximum && isLocalMaximum) {
            pvelIndex = i;
            y_PVEL1   = currVal;
            lm.PVEL1  = i / sr;
            break;
        }
        if (desired == ExtremumType::LocalMinimum && isLocalMinimum) {
            pvelIndex = i;
            y_PVEL1   = currVal;
            lm.PVEL1  = i / sr;
            break;
        }
    }
    if (pvelIndex == -1 || !std::isfinite(y_PVEL1))
        return;

    // 4) Sign-normalized velocity: "toward target" is always positive.
    //    Threshold is 20% of |PVEL1|.
    double sign = (y_PVEL1 >= 0.0 ? 1.0 : -1.0);
    auto vNorm = [&](int idx) -> double {
        return sign * velocityData[idx];
    };
    double peakMag  = sign * y_PVEL1;   // > 0
    double threshold = 0.2 * peakMag;   // 20% of peak closing speed

    // 5) GONS: first time *before* PVEL1 where v crosses +threshold upward
    //    (acceleration toward target).
    double gons = std::numeric_limits<double>::quiet_NaN();
    for (int i = pvelIndex; i >= 1; --i) {
        double prevNorm = vNorm(i - 1);
        double currNorm = vNorm(i);

        if (!std::isfinite(prevNorm) || !std::isfinite(currNorm))
            continue;

        if (prevNorm < threshold && currNorm >= threshold) {
            double t1   = (i - 1) / sr;
            double t2   = i / sr;
            double frac = (threshold - prevNorm) / (currNorm - prevNorm);
            gons = t1 + frac * (t2 - t1);
            break;
        }
    }
    lm.GONS = gons;

    // 6) NONS: first time *after* PVEL1 where v crosses +threshold downward
    //    (deceleration toward target into the constriction plateau).
    double nons = std::numeric_limits<double>::quiet_NaN();
    for (int i = pvelIndex; i < static_cast<int>(velocityData.size()) - 1; ++i) {
        double currNorm = vNorm(i);
        double nextNorm = vNorm(i + 1);

        if (!std::isfinite(currNorm) || !std::isfinite(nextNorm))
            continue;

        // Crossing from >= 0.2*PVEL1 down to < 0.2*PVEL1
        if (currNorm >= threshold && nextNorm < threshold) {
            double t1   = i / sr;
            double t2   = (i + 1) / sr;
            double frac = (threshold - currNorm) / (nextNorm - currNorm);
            nons = t1 + frac * (t2 - t1);
            break;
        }
    }
    lm.NONS = nons;
}


void Label::findFollowingVelocityExtremumFromVector(Label::landmarks &lm,
                                                    double xCenterSec,
                                                    const std::vector<double> &velocityData,
                                                    double sr)
{
    lm.MaxC = xCenterSec;

    if (!std::isfinite(xCenterSec))
        return;

    if (velocityData.size() < 3)
        return;

    // 1) Find the sample index closest to xCenterSec.
    int nearestIndex = -1;
    double minDist   = std::numeric_limits<double>::max();
    for (int i = 0; i < static_cast<int>(velocityData.size()); ++i) {
        double tSec = i / sr;
        double dist = std::fabs(tSec - xCenterSec);
        if (dist < minDist) {
            minDist    = dist;
            nearestIndex = i;
        }
    }
    if (nearestIndex >= static_cast<int>(velocityData.size()) - 1)
        return;

    // 2) Determine desired extremum type (opening peak) near MaxC.
    enum class ExtremumType { LocalMaximum, LocalMinimum };
    ExtremumType desired;
    if (velocityData[nearestIndex] > velocityData[nearestIndex + 1])
        desired = ExtremumType::LocalMinimum;
    else if (velocityData[nearestIndex] < velocityData[nearestIndex + 1])
        desired = ExtremumType::LocalMaximum;
    else {
        int j = nearestIndex;
        while (j < static_cast<int>(velocityData.size()) - 1 &&
               velocityData[j] == velocityData[j + 1])
            ++j;
        if (j == static_cast<int>(velocityData.size()) - 1)
            return;
        desired = (velocityData[nearestIndex] > velocityData[j + 1])
                      ? ExtremumType::LocalMinimum
                      : ExtremumType::LocalMaximum;
    }

    // 3) Scan right from nearestIndex to find PVEL2 (peak opening velocity).
    double y_PVEL2   = std::numeric_limits<double>::quiet_NaN();
    int    pvelIndex = -1;
    for (int i = nearestIndex + 1; i < static_cast<int>(velocityData.size()) - 1; ++i) {
        if (i < 1 || i + 1 >= static_cast<int>(velocityData.size()))
            continue;

        double prevVal = velocityData[i - 1];
        double currVal = velocityData[i];
        double nextVal = velocityData[i + 1];

        if (!std::isfinite(currVal))
            continue;

        bool isLocalMaximum = (currVal >= prevVal && currVal > nextVal);
        bool isLocalMinimum = (currVal <= prevVal && currVal < nextVal);

        if (desired == ExtremumType::LocalMaximum && isLocalMaximum) {
            pvelIndex = i;
            y_PVEL2   = currVal;
            lm.PVEL2  = i / sr;
            break;
        }
        if (desired == ExtremumType::LocalMinimum && isLocalMinimum) {
            pvelIndex = i;
            y_PVEL2   = currVal;
            lm.PVEL2  = i / sr;
            break;
        }
    }
    if (pvelIndex == -1 || !std::isfinite(y_PVEL2))
        return;

    // 4) Sign-normalized velocity: "away from target" is always positive.
    //    Threshold is 20% of |PVEL2|.
    double sign = (y_PVEL2 >= 0.0 ? 1.0 : -1.0);
    auto vNorm = [&](int idx) -> double {
        return sign * velocityData[idx];
    };
    double peakMag  = sign * y_PVEL2;   // > 0
    double threshold = 0.2 * peakMag;   // 20% of peak opening speed

    const int N = static_cast<int>(velocityData.size());

    // 5) NOFFS: first time between MaxC and PVEL2 where v crosses +threshold upward
    //    (acceleration away from the target, out of the constriction).
    double noffs = std::numeric_limits<double>::quiet_NaN();
    int start = std::max(nearestIndex, 1);   // stay in-bounds and at/after MaxC
    if (pvelIndex > start) {
        for (int i = start; i < pvelIndex; ++i) {
            if (i + 1 >= N)
                break;

            double currNorm = vNorm(i);
            double nextNorm = vNorm(i + 1);
            if (!std::isfinite(currNorm) || !std::isfinite(nextNorm))
                continue;

            // Crossing from < 0.2*PVEL2 up to >= 0.2*PVEL2
            if (currNorm < threshold && nextNorm >= threshold) {
                double t1   = i / sr;
                double t2   = (i + 1) / sr;
                double frac = (threshold - currNorm) / (nextNorm - currNorm);
                noffs = t1 + frac * (t2 - t1);
                break;
            }
        }
    }
    lm.NOFFS = noffs;

    // 6) GOFFS: first time after PVEL2 where v crosses +threshold downward
    //    (deceleration away from the target).
    double goffs = std::numeric_limits<double>::quiet_NaN();
    for (int i = pvelIndex; i < N - 1; ++i) {
        double currNorm = vNorm(i);
        double nextNorm = vNorm(i + 1);
        if (!std::isfinite(currNorm) || !std::isfinite(nextNorm))
            continue;

        // Crossing from > 0.2*PVEL2 down to <= 0.2*PVEL2
        if (currNorm > threshold && nextNorm <= threshold) {
            double t1   = i / sr;
            double t2   = (i + 1) / sr;
            double frac = (threshold - currNorm) / (nextNorm - currNorm);
            goffs = t1 + frac * (t2 - t1);
            break;
        }
    }
    lm.GOFFS = goffs;
}

void Label::placeLabelAt(double x, QString labelName)
{

    if (!m_plot) return;

    static const double DUPLICATE_EPSILON = 1e-3;
    for (auto *existingLine : m_labelLines) {
        const double existingX = existingLine->start->coords().x();
        if (qAbs(existingX - x) < DUPLICATE_EPSILON) return;
    }

    if (m_plot->property("isSpectrogram").toBool())
        return;

    QCPItemLine *labelLine = new QCPItemLine(m_plot);
    labelLine->setPen(QPen(Qt::black));
    labelLine->start->setCoords(x, m_plot->yAxis->range().lower);
    labelLine->end  ->setCoords(x, m_plot->yAxis->range().upper);
    labelLine->setSelectable(true);
    m_labelLines.append(labelLine);

    if (labelName.isEmpty()) {
        if (auto *mw = qobject_cast<mainWindow*>(m_plot->window())) {
            const int nextCount = mw->getGlobalNameCounter();
            labelName = QString("name%1").arg(nextCount);
            mw->incrementGlobalNameCounter();
        }
    }
    m_labelLineNameMap[labelLine] = labelName;

    QCPItemText *labelText = new QCPItemText(m_plot);
    labelText->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    labelText->position->setCoords(x, m_plot->yAxis->range().lower);
    labelText->setText(QString::number(x, 'f', 2));
    labelText->setFont(QFont("Arial", 8));
    labelText->setColor(Qt::black);
    labelText->setSelectable(false);
    m_labelTexts.append(labelText);

    MyQCPItemText *handleText = new MyQCPItemText(m_plot);
    handleText->setSelectable(true);
    handleText->setText(QString::fromUtf8("▇"));
    handleText->setColor(Qt::black);
    handleText->setFont(QFont("Arial", 12));
    handleText->setClipToAxisRect(false);
    handleText->setClipAxisRect(nullptr);
    handleText->setLayer("overlay");
    const double yTop = m_plot->yAxis->range().upper;
    handleText->position->setType(QCPItemPosition::ptPlotCoords);
    handleText->position->setCoords(x, yTop);
    handleText->setPositionAlignment(Qt::AlignHCenter | Qt::AlignCenter);

    m_labelHandleMapText[handleText] = labelLine;
    m_lineToBottomText[labelLine]    = labelText;

    QString legendName = (m_plot->graphCount() > 0 && !m_plot->graph(0)->name().isEmpty())
                             ? m_plot->graph(0)->name()
                             : QStringLiteral("Landmark");
    m_labelLineLegendMap[labelLine] = legendName;

    emit landmarkAdded(legendName, x, labelName);

    m_plot->replot();
}

void Label::removeLabelAt(double x, const QString &legendName)
{
    if (!m_plot || m_labelLines.isEmpty()) return;

    // Local helpers (idempotent removal)
    auto plotHasItem = [this](QCPAbstractItem* it)->bool {
        if (!m_plot || !it) return false;
        for (int i = 0, n = m_plot->itemCount(); i < n; ++i)
            if (m_plot->item(i) == it) return true;
        return false;
    };
    auto safeRemove = [&](QCPAbstractItem* it){
        if (plotHasItem(it)) m_plot->removeItem(it);
    };

    // Find closest line to x
    auto closestLineIt = std::min_element(
        m_labelLines.begin(), m_labelLines.end(),
        [x](QCPItemLine *l1, QCPItemLine *l2){
            return std::fabs(l1->start->coords().x() - x)
            < std::fabs(l2->start->coords().x() - x);
        });
    if (closestLineIt == m_labelLines.end()) return;

    QCPItemLine *line = *closestLineIt;
    const double lx   = line->start->coords().x();

    if (m_labelLineLegendMap.value(line) == legendName)
    {
        // Clear any selection/drag state tied to this line
        if (m_currentlySelectedLine == line) m_currentlySelectedLine = nullptr;
        if (m_draggedLine == line) { m_isDragging = false; m_draggedHandle = nullptr; m_draggedLine = nullptr; }

        // Remove top-handle(s) tied to this line
        for (auto it = m_labelHandleMapText.begin(); it != m_labelHandleMapText.end(); ) {
            if (it.value() == line) {
                safeRemove(it.key());
                it = m_labelHandleMapText.erase(it);
            } else {
                ++it;
            }
        }

        // Remove bottom text mapped to this line
        if (QCPItemText* bottom = m_lineToBottomText.take(line)) {
            safeRemove(bottom);
            m_labelTexts.removeOne(bottom);
        }

        // Clean maps and remove the line
        m_labelLineNameMap.remove(line);
        m_labelLineLegendMap.remove(line);
        safeRemove(line);
        m_labelLines.erase(closestLineIt);

        // Notify model
        emit landmarkRemoved(lx);
    }

    m_plot->replot();
}

void Label::removeAllLabels()
{
    // Clear any selection/dragging state
    m_currentlySelectedLine = nullptr;
    m_isDragging = false;
    m_draggedHandle = nullptr;
    m_draggedLine = nullptr;

    // Local helpers (idempotent removal)
    auto plotHasItem = [this](QCPAbstractItem* it)->bool {
        if (!m_plot || !it) return false;
        for (int i = 0, n = m_plot->itemCount(); i < n; ++i)
            if (m_plot->item(i) == it) return true;
        return false;
    };
    auto safeRemove = [&](QCPAbstractItem* it){
        if (plotHasItem(it)) m_plot->removeItem(it);
    };

    // Remove each line and its paired UI
    while (!m_labelLines.isEmpty()) {
        QCPItemLine *line = m_labelLines.takeFirst();
        const double x = line->start->coords().x();

        // Remove handles tied to this line
        for (auto it = m_labelHandleMapText.begin(); it != m_labelHandleMapText.end(); ) {
            if (it.value() == line) {
                safeRemove(it.key());
                it = m_labelHandleMapText.erase(it);
            } else {
                ++it;
            }
        }

        // Remove bottom text mapped to this line
        if (QCPItemText* bottom = m_lineToBottomText.take(line)) {
            safeRemove(bottom);
            m_labelTexts.removeOne(bottom);
        }

        // Clean maps and remove the line
        m_labelLineNameMap.remove(line);
        m_labelLineLegendMap.remove(line);
        safeRemove(line);

        emit landmarkRemoved(x);
    }

    // Safety passes (in case anything remained)
    for (QCPItemText* text : m_labelTexts) safeRemove(text);
    m_labelTexts.clear();

    for (auto it = m_labelHandleMapText.begin(); it != m_labelHandleMapText.end(); ++it)
        safeRemove(it.key());
    m_labelHandleMapText.clear();

    m_lineToBottomText.clear();
    m_labelLineNameMap.clear();
    m_labelLineLegendMap.clear();

    if (m_plot) m_plot->replot();
}

void Label::onItemClicked(QCPAbstractItem *item, QMouseEvent *event)
{
    Q_UNUSED(event);

    QCPItemText* clickedText = qobject_cast<QCPItemText*>(item);
    if (!clickedText) return;

    if (m_labelHandleMapText.contains(clickedText))
    {
        QCPItemLine* line = m_labelHandleMapText[clickedText];

        // If an old selection exists but the line has been removed, just drop it.
        if (m_currentlySelectedLine && !m_labelLines.contains(m_currentlySelectedLine))
            m_currentlySelectedLine = nullptr;

        if (m_currentlySelectedLine && m_currentlySelectedLine != line)
            clearSelectedLine();

        if (!m_labelLines.contains(line)) return; // removed elsewhere

        line->setPen(QPen(Qt::red, 1.5));
        m_currentlySelectedLine = line;

        clickedText->setColor(Qt::red);

        const QString legendName = m_labelLineLegendMap.value(line);
        const double  xCoord     = line->start->coords().x();
        emit labelClicked(legendName, xCoord);

        m_plot->replot();
    }
}

void Label::clearSelectedLine()
{
    if (!m_currentlySelectedLine) return;

    // Only act if this pointer still corresponds to a live line we track
    if (!m_labelLines.contains(m_currentlySelectedLine)) {
        m_currentlySelectedLine = nullptr;
        return;
    }

    m_currentlySelectedLine->setPen(QPen(Qt::black, 1));
    for (auto it = m_labelHandleMapText.begin(); it != m_labelHandleMapText.end(); ++it) {
        if (it.value() == m_currentlySelectedLine)
            it.key()->setColor(Qt::black);
    }
    m_currentlySelectedLine = nullptr;
    m_plot->replot();
}

bool Label::selectLineIfMatch(const QString &legendName, double xOffset)
{
    for (auto it = m_labelLineLegendMap.constBegin(); it != m_labelLineLegendMap.constEnd(); ++it) {
        QCPItemLine *line = it.key();
        const QString lineLegend = it.value();
        const double  lineX      = line->start->coords().x();

        if (lineLegend == legendName && std::fabs(lineX - xOffset) < 1e-3) {
            clearSelectedLine();
            if (!m_labelLines.contains(line)) return false;
            line->setPen(QPen(Qt::red, 1.5));
            for (auto h = m_labelHandleMapText.constBegin(); h != m_labelHandleMapText.constEnd(); ++h) {
                if (h.value() == line) h.key()->setColor(Qt::red);
            }
            m_currentlySelectedLine = line;
            m_plot->replot();
            return true;
        }
    }
    return false;
}

void Label::onMousePress(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) return;

    QCPAbstractItem* itemUnderMouse = m_plot->itemAt(event->pos(), true);
    if (!itemUnderMouse) return;

    QCPItemText* handleText = qobject_cast<QCPItemText*>(itemUnderMouse);
    if (!handleText) return;
    if (!m_labelHandleMapText.contains(handleText)) return;

    m_isDragging   = true;
    m_draggedHandle= handleText;
    m_draggedLine  = m_labelHandleMapText[handleText];

    // If the line got removed somehow, abort
    if (!m_labelLines.contains(m_draggedLine)) {
        m_isDragging = false;
        m_draggedHandle = nullptr;
        m_draggedLine = nullptr;
        return;
    }

    m_initialClickX = m_plot->xAxis->pixelToCoord(event->pos().x());
    m_initialLineX  = m_draggedLine->start->coords().x();

    m_draggedLine->setPen(QPen(Qt::green, 1.5));
    m_draggedHandle->setColor(Qt::green);
    m_plot->replot();
}

void Label::onMouseMove(QMouseEvent* event)
{
    if (!m_isDragging || !m_draggedLine || !m_draggedHandle) return;
    if (!m_labelLines.contains(m_draggedLine)) { // line deleted while dragging
        m_isDragging = false;
        m_draggedHandle = nullptr;
        m_draggedLine = nullptr;
        return;
    }

    const double currentX = m_plot->xAxis->pixelToCoord(event->pos().x());
    const double deltaX   = currentX - m_initialClickX;
    const double newLineX = m_initialLineX + deltaX;

    m_draggedLine->start->setCoords(newLineX, m_plot->yAxis->range().lower);
    m_draggedLine->end  ->setCoords(newLineX, m_plot->yAxis->range().upper);

    const double yTop = m_plot->yAxis->range().upper;
    m_draggedHandle->position->setCoords(newLineX, yTop);

    if (QCPItemText* bottomText = m_lineToBottomText.value(m_draggedLine, nullptr)) {
        bottomText->position->setCoords(newLineX, m_plot->yAxis->range().lower);
        bottomText->setText(QString::number(newLineX, 'f', 2));
    }

    m_plot->replot();
}

void Label::onMouseRelease(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton || !m_isDragging || !m_draggedLine)
        return;

    // If the line was removed during drag, just reset state
    if (!m_labelLines.contains(m_draggedLine)) {
        m_isDragging = false;
        m_draggedHandle = nullptr;
        m_draggedLine = nullptr;
        return;
    }

    m_isDragging = false;

    QCPItemLine* draggedLine = m_draggedLine;
    const QString legendName = m_labelLineLegendMap.value(draggedLine);
    const QString thisLabel  = m_labelLineNameMap.value(draggedLine);
    const double  oldX       = m_initialLineX;
    const double  finalX     = draggedLine->start->coords().x();

    m_draggedHandle = nullptr;
    m_draggedLine   = nullptr;

    // Emit synchronously to avoid dangling captures if items get deleted immediately after
    emit labelMoved(legendName, thisLabel, finalX, oldX);
}

void Label::moveHandleBy(QCPItemText *handle, double deltaX)
{
    if (!handle || !m_labelHandleMapText.contains(handle)) return;

    QCPItemLine* line = m_labelHandleMapText.value(handle);
    if (!m_labelLines.contains(line)) return;

    const double oldX = line->start->coords().x();
    const double newX = oldX + deltaX;

    line->start->setCoords(newX, m_plot->yAxis->range().lower);
    line->end  ->setCoords(newX, m_plot->yAxis->range().upper);

    const double yTop = m_plot->yAxis->range().upper;
    handle->position->setCoords(newX, yTop);

    if (QCPItemText* bottomText = m_lineToBottomText.value(line, nullptr)) {
        bottomText->position->setCoords(newX, m_plot->yAxis->range().lower);
        bottomText->setText(QString::number(newX, 'f', 2));
    }

    m_plot->replot();
}

void Label::updateLabelName(double x, const QString &actualName)
{
    const double EPS = 1e-3;
    for (QCPItemLine *line : m_labelLines) {
        const double lx = line->start->coords().x();
        if (std::abs(lx - x) < EPS) {
            m_labelLineNameMap[line] = actualName;
            return;
        }
    }
}

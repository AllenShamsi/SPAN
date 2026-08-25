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

            const int dimCount = countTrailingLowerCase(legendName);
            const bool isSingleChannel = (dimCount == 1);

            const bool startsWithDerivedParameter =
                legendName.startsWith(QLatin1Char('v')) ||
                legendName.startsWith(QLatin1Char('a')) ||
                legendName.startsWith(QLatin1Char('i'));

            const bool isTwoDimensionalGesture =
                (dimCount == 2 && !startsWithDerivedParameter);

            // One dimension: old behavior.
            // Two dimensions: new tangential gesture behavior.
            // Three dimensions or anything else: manual label only for now.
            if (!isSingleChannel && !isTwoDimensionalGesture) {
                placeLabelAt(clickedX, QString());
                return;
            }

            if (auto *mw = qobject_cast<mainWindow*>(m_plot->window())) {
                const double sr = mw->getCurrentSpanWavSR();

                std::vector<double> velocityData;
                double centerX = clickedX;

                if (isSingleChannel) {
                    // Existing behavior:
                    // snap MaxC to the nearest visible displacement extremum,
                    // then use the matching signed single-axis velocity.
                    centerX = findNearestPeak(clickedX, legendName);
                    velocityData = mw->getPrecomputedVelocity(legendName);
                } else {
                    // New behavior for TTxz / TTxy / TTyz:
                    // keep the visible position traces, but use hidden tangential speed
                    // as the gesture-finding basis.
                    velocityData = mw->getPrecomputedTangentialVelocity(legendName);

                    if (!velocityData.empty())
                        centerX = findNearestVelocityMinimum(clickedX, velocityData, sr);
                }

                placeLabelAt(centerX, QStringLiteral("MaxC"));

                if (!velocityData.empty()) {
                    landmarks lm;

                    if (isTwoDimensionalGesture) {
                        findTangentialGestureLandmarksFromSpeed(lm, centerX, velocityData, sr);
                    } else {
                        findPrecedingVelocityExtremumFromVector(lm, centerX, velocityData, sr);
                        findFollowingVelocityExtremumFromVector (lm, centerX, velocityData, sr);
                    }

                    if (std::isfinite(lm.GONS))  placeLabelAt(lm.GONS,  QStringLiteral("GONS"));
                    if (std::isfinite(lm.NONS))  placeLabelAt(lm.NONS,  QStringLiteral("NONS"));
                    if (std::isfinite(lm.PVEL1)) emit landmarkAdded(legendName, lm.PVEL1, QStringLiteral("PVEL1"));
                    if (std::isfinite(lm.PVEL2)) emit landmarkAdded(legendName, lm.PVEL2, QStringLiteral("PVEL2"));
                    if (std::isfinite(lm.NOFFS)) placeLabelAt(lm.NOFFS, QStringLiteral("NOFFS"));
                    if (std::isfinite(lm.GOFFS)) placeLabelAt(lm.GOFFS, QStringLiteral("GOFFS"));
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
inline bool crossesUpInclusive(double y0, double y1, double threshold)
{
    return (y0 <= threshold && y1 >  threshold) ||
           (y0 <  threshold && y1 >= threshold);
}

inline bool crossesDownInclusive(double y0, double y1, double threshold)
{
    return (y0 >= threshold && y1 <  threshold) ||
           (y0 >  threshold && y1 <= threshold);
}

inline double interpolateThresholdCrossing(int i0,
                                           int i1,
                                           double y0,
                                           double y1,
                                           double threshold,
                                           double sr)
{
    const double t0 = i0 / sr;
    const double t1 = i1 / sr;

    if (!std::isfinite(y0) || !std::isfinite(y1) || sr <= 0.0)
        return std::numeric_limits<double>::quiet_NaN();

    if (std::fabs(y1 - y0) < 1e-12)
        return t0;

    const double frac = (threshold - y0) / (y1 - y0);
    return t0 + frac * (t1 - t0);
}
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

void Label::setVelocityThresholdFraction(double value)
{
    if (std::isfinite(value) && value > 0.0 && value < 1.0)
        m_velocityThresholdFraction = value;
}

void Label::setReopeningDeadbandFraction(double value)
{
    if (std::isfinite(value) && value >= 0.0 && value < 1.0)
        m_reopeningDeadbandFraction = value;
}


int Label::countTrailingLowerCase(const QString &str)
{
    int count = 0;

    for (int i = str.size() - 1; i >= 0; --i) {
        const QChar c = str.at(i).toLower();

        if (c == QLatin1Char('x') ||
            c == QLatin1Char('y') ||
            c == QLatin1Char('z'))
        {
            ++count;
        } else {
            break;
        }
    }

    return count;
}

double Label::findNearestVelocityMinimum(double xClicked,
                                         const std::vector<double> &velocityData,
                                         double sr) const
{
    if (velocityData.size() < 3 || sr <= 0.0 || !std::isfinite(xClicked))
        return xClicked;

    const int N = static_cast<int>(velocityData.size());

    int clickedIndex = static_cast<int>(std::round(xClicked * sr));
    clickedIndex = std::max(1, std::min(clickedIndex, N - 2));

    // Search within 25% of the visible window.
    // This keeps MaxC near the user click rather than jumping to an unrelated valley.
    int searchRadius =
        static_cast<int>(std::round(0.25 * m_plot->xAxis->range().size() * sr));

    if (searchRadius <= 0)
        searchRadius = static_cast<int>(std::round(0.25 * sr)); // fallback: 250 ms

    const int start = std::max(1, clickedIndex - searchRadius);
    const int end   = std::min(N - 2, clickedIndex + searchRadius);

    bool haveLocalMin = false;
    int bestIndex = clickedIndex;
    double bestDistance = std::numeric_limits<double>::max();

    for (int i = start; i <= end; ++i) {
        const double prevVal = velocityData[static_cast<size_t>(i - 1)];
        const double currVal = velocityData[static_cast<size_t>(i)];
        const double nextVal = velocityData[static_cast<size_t>(i + 1)];

        if (!std::isfinite(prevVal) ||
            !std::isfinite(currVal) ||
            !std::isfinite(nextVal))
            continue;

        const bool isLocalMinimum = (currVal <= prevVal && currVal <= nextVal);
        if (!isLocalMinimum)
            continue;

        const double d = std::fabs(static_cast<double>(i - clickedIndex));

        if (!haveLocalMin || d < bestDistance) {
            haveLocalMin = true;
            bestDistance = d;
            bestIndex = i;
        }
    }

    if (haveLocalMin)
        return bestIndex / sr;

    // Fallback: choose the lowest-speed sample in the local window.
    double bestValue = std::numeric_limits<double>::max();

    for (int i = start; i <= end; ++i) {
        const double v = velocityData[static_cast<size_t>(i)];

        if (!std::isfinite(v))
            continue;

        if (v < bestValue) {
            bestValue = v;
            bestIndex = i;
        }
    }

    return bestIndex / sr;
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

    if (velocityData.size() < 3 || sr <= 0.0)
        return;

    const int N = static_cast<int>(velocityData.size());

    // 1) Find the sample index closest to MaxC.
    int nearestIndex = -1;
    double minDist = std::numeric_limits<double>::max();

    for (int i = 0; i < N; ++i) {
        const double tSec = i / sr;
        const double dist = std::fabs(tSec - xCenterSec);
        if (dist < minDist) {
            minDist = dist;
            nearestIndex = i;
        }
    }

    if (nearestIndex < 2)
        return;

    // Search within a reasonable window before MaxC.
    // This avoids letting a distant unrelated movement define the deadband.
    const int searchWindowSamples = static_cast<int>(std::round(1.0 * sr)); // 1 second
    const int searchStart = std::max(1, nearestIndex - searchWindowSamples);

    // 2) Estimate a deadband from the pre-MaxC velocity region.
    double maxAbsPast = 0.0;
    for (int i = searchStart; i < nearestIndex; ++i) {
        if (std::isfinite(velocityData[i])) {
            maxAbsPast = std::max(maxAbsPast, std::fabs(velocityData[i]));
        }
    }

    if (maxAbsPast <= 0.0)
        return;

    // Same idea as PVEL2: ignore tiny wiggles near zero.
    const double deadband = m_reopeningDeadbandFraction * maxAbsPast;

    // 3) Scan backward from MaxC.
    //    Once we leave the near-zero region, we are inside the closing excursion.
    //    Keep the strongest local extremum in that excursion.
    double y_PVEL1 = std::numeric_limits<double>::quiet_NaN();
    int pvelIndex = -1;

    double strongestSampleValue = std::numeric_limits<double>::quiet_NaN();
    int strongestSampleIndex = -1;

    bool inClosingExcursion = false;
    bool foundLocalCandidate = false;

    for (int i = nearestIndex - 1; i >= searchStart; --i) {
        if (i < 1 || i + 1 >= N)
            continue;

        const double prevVal = velocityData[i - 1];
        const double currVal = velocityData[i];
        const double nextVal = velocityData[i + 1];

        if (!std::isfinite(prevVal) ||
            !std::isfinite(currVal) ||
            !std::isfinite(nextVal))
            continue;

        const double currAbs = std::fabs(currVal);

        // Wait until the signal clearly leaves the zero/plateau region.
        if (!inClosingExcursion) {
            if (currAbs > deadband)
                inClosingExcursion = true;
            else
                continue;
        }

        // Track strongest sample as a fallback.
        if (strongestSampleIndex == -1 ||
            currAbs > std::fabs(strongestSampleValue))
        {
            strongestSampleIndex = i;
            strongestSampleValue = currVal;
        }

        const bool isLocalMaximum = (currVal > prevVal && currVal >= nextVal);
        const bool isLocalMinimum = (currVal < prevVal && currVal <= nextVal);
        const bool isLocalExtremum = isLocalMaximum || isLocalMinimum;

        if (isLocalExtremum) {
            if (!foundLocalCandidate ||
                currAbs > std::fabs(y_PVEL1))
            {
                foundLocalCandidate = true;
                pvelIndex = i;
                y_PVEL1 = currVal;
            }
        }

        // Since we are scanning backward, once we have found a candidate and
        // return to the deadband, we have reached the pre-gesture side.
        if ((foundLocalCandidate || strongestSampleIndex != -1) &&
            currAbs <= deadband)
        {
            break;
        }
    }

    // Fallback: if no formal local extremum was found, use the strongest sample.
    if (pvelIndex == -1 && strongestSampleIndex != -1) {
        pvelIndex = strongestSampleIndex;
        y_PVEL1 = strongestSampleValue;
    }

    if (pvelIndex == -1 || !std::isfinite(y_PVEL1))
        return;

    lm.PVEL1 = pvelIndex / sr;

    // 4) Sign-normalized velocity: "toward target" is always positive.
    const double sign = (y_PVEL1 >= 0.0 ? 1.0 : -1.0);

    auto vNorm = [&](int idx) -> double {
        return sign * velocityData[idx];
    };

    const double peakMag = sign * y_PVEL1;  // positive
    const double threshold = m_velocityThresholdFraction * peakMag;

    // 5) GONS: first time before PVEL1 where velocity crosses threshold upward.
    double gons = std::numeric_limits<double>::quiet_NaN();

    for (int i = pvelIndex; i >= searchStart + 1; --i) {
        const double prevNorm = vNorm(i - 1);
        const double currNorm = vNorm(i);

        if (!std::isfinite(prevNorm) || !std::isfinite(currNorm))
            continue;

        if (crossesUpInclusive(prevNorm, currNorm, threshold)) {
            gons = interpolateThresholdCrossing(i - 1, i,
                                                prevNorm, currNorm,
                                                threshold, sr);
            break;
        }
    }

    // Fallback: if PVEL1 exists but no clean threshold crossing was found,
    // still place GONS at the left edge of the local pre-MaxC search window.
    lm.GONS = std::isfinite(gons) ? gons : (searchStart / sr);

    // 6) NONS: first time after PVEL1 where velocity crosses threshold downward.
    double nons = std::numeric_limits<double>::quiet_NaN();

    for (int i = pvelIndex; i < nearestIndex; ++i) {
        const double currNorm = vNorm(i);
        const double nextNorm = vNorm(i + 1);

        if (!std::isfinite(currNorm) || !std::isfinite(nextNorm))
            continue;

        if (crossesDownInclusive(currNorm, nextNorm, threshold)) {
            nons = interpolateThresholdCrossing(i, i + 1,
                                                currNorm, nextNorm,
                                                threshold, sr);
            break;
        }
    }

    // Fallback: if PVEL1 exists but no clean target-onset crossing was found,
    // place NONS at MaxC.
    lm.NONS = std::isfinite(nons) ? nons : xCenterSec;
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

    // 3) Find the main reopening peak (PVEL2), not just the first tiny extremum.
    //    Strategy:
    //    - estimate a small deadband around zero from the future opening-side signal
    //    - wait until velocity clearly leaves that deadband in the reopening direction
    //    - within that opening excursion, keep the strongest local extremum
    double y_PVEL2   = std::numeric_limits<double>::quiet_NaN();
    int    pvelIndex = -1;

    double maxAbsFuture = 0.0;
    for (int i = nearestIndex + 1; i < static_cast<int>(velocityData.size()); ++i) {
        if (std::isfinite(velocityData[i])) {
            maxAbsFuture = std::max(maxAbsFuture, std::fabs(velocityData[i]));
        }
    }

    if (maxAbsFuture <= 0.0)
        return;

    // Deadband to ignore tiny post-MaxC wiggles near zero.
    // 5% is the default.
    const double deadband = m_reopeningDeadbandFraction * maxAbsFuture;

    bool inOpeningExcursion = false;
    bool foundCandidate     = false;

    for (int i = nearestIndex + 1; i < static_cast<int>(velocityData.size()) - 1; ++i) {
        if (i < 1 || i + 1 >= static_cast<int>(velocityData.size()))
            continue;

        double prevVal = velocityData[i - 1];
        double currVal = velocityData[i];
        double nextVal = velocityData[i + 1];

        if (!std::isfinite(prevVal) || !std::isfinite(currVal) || !std::isfinite(nextVal))
            continue;

        // Wait until the signal clearly leaves the zero region
        // in the reopening direction.
        if (!inOpeningExcursion) {
            if (desired == ExtremumType::LocalMaximum) {
                if (currVal > deadband)
                    inOpeningExcursion = true;
                else
                    continue;
            } else { // LocalMinimum
                if (currVal < -deadband)
                    inOpeningExcursion = true;
                else
                    continue;
            }
        }

        bool isLocalMaximum = (currVal >= prevVal && currVal > nextVal);
        bool isLocalMinimum = (currVal <= prevVal && currVal < nextVal);

        if (desired == ExtremumType::LocalMaximum && isLocalMaximum) {
            if (!foundCandidate || currVal > y_PVEL2) {
                foundCandidate = true;
                pvelIndex = i;
                y_PVEL2   = currVal;
            }
        }

        if (desired == ExtremumType::LocalMinimum && isLocalMinimum) {
            if (!foundCandidate || currVal < y_PVEL2) {
                foundCandidate = true;
                pvelIndex = i;
                y_PVEL2   = currVal;
            }
        }

        // Once we've found at least one reopening candidate, stop when the signal
        // returns to the deadband region. That keeps us within the same opening excursion
        // and avoids drifting into later unrelated movements.
        if (foundCandidate) {
            if (desired == ExtremumType::LocalMaximum && currVal <= deadband)
                break;
            if (desired == ExtremumType::LocalMinimum && currVal >= -deadband)
                break;
        }
    }

    if (pvelIndex == -1 || !std::isfinite(y_PVEL2))
        return;

    lm.PVEL2 = pvelIndex / sr;

    // 4) Sign-normalized velocity: "away from target" is always positive.
    //    Threshold is 20% of |PVEL2|.
    double sign = (y_PVEL2 >= 0.0 ? 1.0 : -1.0);
    auto vNorm = [&](int idx) -> double {
        return sign * velocityData[idx];
    };
    double peakMag  = sign * y_PVEL2;   // > 0
    double threshold = m_velocityThresholdFraction * peakMag;   // default is 20% of peak opening speed

    const int N = static_cast<int>(velocityData.size());

    // 5) NOFFS: first time between MaxC and PVEL2 where v crosses +threshold upward
    //    (acceleration away from the target, out of the constriction).
    double noffs = std::numeric_limits<double>::quiet_NaN();

    const int start = std::max(nearestIndex, 1);

    if (pvelIndex > start) {
        for (int i = start; i < pvelIndex; ++i) {
            if (i + 1 >= N)
                break;

            const double currNorm = vNorm(i);
            const double nextNorm = vNorm(i + 1);

            if (!std::isfinite(currNorm) || !std::isfinite(nextNorm))
                continue;

            if (crossesUpInclusive(currNorm, nextNorm, threshold)) {
                noffs = interpolateThresholdCrossing(i, i + 1,
                                                     currNorm, nextNorm,
                                                     threshold, sr);
                break;
            }
        }
    }

    // Fallback: if PVEL2 exists but no clean release-onset crossing was found,
    // place NOFFS at MaxC.
    lm.NOFFS = std::isfinite(noffs) ? noffs : xCenterSec;

    // 6) GOFFS: first time after PVEL2 where v crosses +threshold downward
    //    (deceleration away from the target).
    double goffs = std::numeric_limits<double>::quiet_NaN();

    for (int i = pvelIndex; i < N - 1; ++i) {
        const double currNorm = vNorm(i);
        const double nextNorm = vNorm(i + 1);

        if (!std::isfinite(currNorm) || !std::isfinite(nextNorm))
            continue;

        if (crossesDownInclusive(currNorm, nextNorm, threshold)) {
            goffs = interpolateThresholdCrossing(i, i + 1,
                                                 currNorm, nextNorm,
                                                 threshold, sr);
            break;
        }
    }

    // Fallback: if PVEL2 exists but no clean gesture-offset crossing was found,
    // place GOFFS at the end of the signal.
    lm.GOFFS = std::isfinite(goffs) ? goffs : ((N - 1) / sr);
}

void Label::findTangentialGestureLandmarksFromSpeed(Label::landmarks &lm,
                                                    double xCenterSec,
                                                    const std::vector<double> &speedData,
                                                    double sr)
{
    lm.MaxC = xCenterSec;

    if (!std::isfinite(xCenterSec) || speedData.size() < 5 || sr <= 0.0)
        return;

    const int N = static_cast<int>(speedData.size());

    int centerIndex = static_cast<int>(std::round(xCenterSec * sr));
    centerIndex = std::max(2, std::min(centerIndex, N - 3));

    auto finiteSpeed = [&](int idx) -> double {
        if (idx < 0 || idx >= N)
            return std::numeric_limits<double>::quiet_NaN();

        const double v = speedData[static_cast<size_t>(idx)];
        return std::isfinite(v) ? v : std::numeric_limits<double>::quiet_NaN();
    };

    auto isLocalMax = [&](int idx) -> bool {
        const double prev = finiteSpeed(idx - 1);
        const double curr = finiteSpeed(idx);
        const double next = finiteSpeed(idx + 1);

        if (!std::isfinite(prev) ||
            !std::isfinite(curr) ||
            !std::isfinite(next))
            return false;

        return curr >= prev && curr >= next &&
               (curr > prev || curr > next);
    };

    auto interpolateCrossing = [&](int i0, int i1, double threshold) -> double {
        const double y0 = finiteSpeed(i0);
        const double y1 = finiteSpeed(i1);

        const double t0 = i0 / sr;
        const double t1 = i1 / sr;

        if (!std::isfinite(y0) || !std::isfinite(y1))
            return std::numeric_limits<double>::quiet_NaN();

        if (std::fabs(y1 - y0) < 1e-12)
            return t0;

        const double frac = (threshold - y0) / (y1 - y0);
        return t0 + frac * (t1 - t0);
    };

    const double centerSpeed = finiteSpeed(centerIndex);

    if (!std::isfinite(centerSpeed))
        return;

    // Limit how far the algorithm is allowed to look away from MaxC.
    // The goal is to find the adjacent closing/opening peaks, not a later
    // unrelated movement.
    double searchSec = 0.30;

    if (m_plot && m_plot->xAxis) {
        const double visibleRange = m_plot->xAxis->range().size();

        if (visibleRange > 0.0) {
            searchSec = 0.25 * visibleRange;
            searchSec = std::max(0.12, std::min(searchSec, 0.30));
        }
    }

    const int searchSamples =
        std::max(5, static_cast<int>(std::round(searchSec * sr)));

    const int leftBound  = std::max(1, centerIndex - searchSamples);
    const int rightBound = std::min(N - 2, centerIndex + searchSamples);

    // Estimate how large a speed rise is available on each side.
    // This is used only to reject tiny wiggles close to MaxC.
    double leftMax = centerSpeed;
    for (int i = centerIndex - 1; i >= leftBound; --i) {
        const double v = finiteSpeed(i);
        if (std::isfinite(v))
            leftMax = std::max(leftMax, v);
    }

    double rightMax = centerSpeed;
    for (int i = centerIndex + 1; i <= rightBound; ++i) {
        const double v = finiteSpeed(i);
        if (std::isfinite(v))
            rightMax = std::max(rightMax, v);
    }

    if (leftMax <= centerSpeed || rightMax <= centerSpeed)
        return;

    // A candidate PVEL must rise at least this much above the MaxC low-speed
    // region. This rejects tiny local bumps.
    //
    // m_velocityThresholdFraction is already 0.20 by default.
    const double minLeftPeak =
        centerSpeed + m_velocityThresholdFraction * (leftMax - centerSpeed);

    const double minRightPeak =
        centerSpeed + m_velocityThresholdFraction * (rightMax - centerSpeed);

    // ------------------------------------------------------------
    // PVEL1: first strong local maximum immediately before MaxC.
    // Scan outward from MaxC to the left. The first qualified peak is the
    // adjacent closing peak. Do NOT choose the strongest peak in the whole window.
    // ------------------------------------------------------------

    int pvel1Index = -1;

    for (int i = centerIndex - 1; i >= leftBound + 1; --i) {
        const double curr = finiteSpeed(i);

        if (!std::isfinite(curr))
            continue;

        if (isLocalMax(i) && curr >= minLeftPeak) {
            pvel1Index = i;
            break;
        }
    }

    // Fallback: if smoothing creates a shoulder/plateau without a formal local max,
    // choose the highest point in the first rising excursion before MaxC.
    if (pvel1Index < 0) {
        bool inExcursion = false;
        int bestIdx = -1;
        double bestVal = -std::numeric_limits<double>::infinity();

        for (int i = centerIndex - 1; i >= leftBound; --i) {
            const double curr = finiteSpeed(i);

            if (!std::isfinite(curr))
                continue;

            if (!inExcursion) {
                if (curr >= minLeftPeak)
                    inExcursion = true;
                else
                    continue;
            }

            if (curr > bestVal) {
                bestVal = curr;
                bestIdx = i;
            }

            // Once we have passed back below the minimum required peak height,
            // stop; this keeps the fallback in the adjacent excursion.
            if (inExcursion && curr < minLeftPeak && bestIdx >= 0)
                break;
        }

        pvel1Index = bestIdx;
    }

    // ------------------------------------------------------------
    // PVEL2: first strong local maximum immediately after MaxC.
    // Scan outward from MaxC to the right.
    // ------------------------------------------------------------

    int pvel2Index = -1;

    for (int i = centerIndex + 1; i <= rightBound - 1; ++i) {
        const double curr = finiteSpeed(i);

        if (!std::isfinite(curr))
            continue;

        if (isLocalMax(i) && curr >= minRightPeak) {
            pvel2Index = i;
            break;
        }
    }

    // Fallback for shoulder/plateau data.
    if (pvel2Index < 0) {
        bool inExcursion = false;
        int bestIdx = -1;
        double bestVal = -std::numeric_limits<double>::infinity();

        for (int i = centerIndex + 1; i <= rightBound; ++i) {
            const double curr = finiteSpeed(i);

            if (!std::isfinite(curr))
                continue;

            if (!inExcursion) {
                if (curr >= minRightPeak)
                    inExcursion = true;
                else
                    continue;
            }

            if (curr > bestVal) {
                bestVal = curr;
                bestIdx = i;
            }

            if (inExcursion && curr < minRightPeak && bestIdx >= 0)
                break;
        }

        pvel2Index = bestIdx;
    }

    if (pvel1Index < 0 || pvel2Index < 0)
        return;

    const double pvel1Value = finiteSpeed(pvel1Index);
    const double pvel2Value = finiteSpeed(pvel2Index);

    if (!std::isfinite(pvel1Value) || !std::isfinite(pvel2Value))
        return;

    lm.PVEL1 = pvel1Index / sr;
    lm.PVEL2 = pvel2Index / sr;

    // Thresholds for target/release around the low-speed MaxC region.
    // For tangential speed, the baseline is centerSpeed, not zero.
    const double threshold1 =
        centerSpeed + m_velocityThresholdFraction * (pvel1Value - centerSpeed);

    const double threshold2 =
        centerSpeed + m_velocityThresholdFraction * (pvel2Value - centerSpeed);

    // ------------------------------------------------------------
    // GONS: crossing upward before PVEL1.
    // ------------------------------------------------------------

    for (int i = pvel1Index; i > leftBound; --i) {
        const double prev = finiteSpeed(i - 1);
        const double curr = finiteSpeed(i);

        if (!std::isfinite(prev) || !std::isfinite(curr))
            continue;

        if (crossesUpInclusive(prev, curr, threshold1)) {
            lm.GONS = interpolateCrossing(i - 1, i, threshold1);
            break;
        }
    }

    // Fallback: if both PVELs were found but no clean GONS crossing was found,
    // place GONS at the left edge of the local search window.
    if (!std::isfinite(lm.GONS))
        lm.GONS = leftBound / sr;

    // ------------------------------------------------------------
    // NONS/TAR: crossing downward after PVEL1 toward MaxC.
    // ------------------------------------------------------------

    for (int i = pvel1Index; i < centerIndex; ++i) {
        const double curr = finiteSpeed(i);
        const double next = finiteSpeed(i + 1);

        if (!std::isfinite(curr) || !std::isfinite(next))
            continue;

        if (crossesDownInclusive(curr, next, threshold1)) {
            lm.NONS = interpolateCrossing(i, i + 1, threshold1);
            break;
        }
    }

    // Fallback: if no clean NONS crossing was found,
    // place NONS at MaxC.
    if (!std::isfinite(lm.NONS))
        lm.NONS = xCenterSec;

    // ------------------------------------------------------------
    // NOFFS/REL: crossing upward after MaxC before PVEL2.
    // ------------------------------------------------------------

    for (int i = centerIndex; i < pvel2Index; ++i) {
        const double curr = finiteSpeed(i);
        const double next = finiteSpeed(i + 1);

        if (!std::isfinite(curr) || !std::isfinite(next))
            continue;

        if (crossesUpInclusive(curr, next, threshold2)) {
            lm.NOFFS = interpolateCrossing(i, i + 1, threshold2);
            break;
        }
    }

    // Fallback: if no clean NOFFS crossing was found,
    // place NOFFS at MaxC.
    if (!std::isfinite(lm.NOFFS))
        lm.NOFFS = xCenterSec;

    // ------------------------------------------------------------
    // GOFFS/OFS: crossing downward after PVEL2.
    // ------------------------------------------------------------

    for (int i = pvel2Index; i < rightBound; ++i) {
        const double curr = finiteSpeed(i);
        const double next = finiteSpeed(i + 1);

        if (!std::isfinite(curr) || !std::isfinite(next))
            continue;

        if (crossesDownInclusive(curr, next, threshold2)) {
            lm.GOFFS = interpolateCrossing(i, i + 1, threshold2);
            break;
        }
    }

    // Fallback: if both PVELs were found but no clean GOFFS crossing was found,
    // place GOFFS at the right edge of the local search window.
    if (!std::isfinite(lm.GOFFS))
        lm.GOFFS = rightBound / sr;
}

void Label::placeLabelAt(double x, QString labelName)
{

    if (!m_plot) return;

    if (m_plot->property("isSpectrogram").toBool())
        return;

    QString legendName = (m_plot->graphCount() > 0 && !m_plot->graph(0)->name().isEmpty())
                             ? m_plot->graph(0)->name()
                             : QStringLiteral("Landmark");

    if (labelName.isEmpty()) {
        if (auto *mw = qobject_cast<mainWindow*>(m_plot->window())) {
            const int nextCount = mw->getGlobalNameCounter();
            labelName = QString("name%1").arg(nextCount);
            mw->incrementGlobalNameCounter();
        }
    }

    static const double DUPLICATE_EPSILON = 1e-6;

    for (auto *existingLine : m_labelLines) {
        if (!existingLine)
            continue;

        const double existingX = existingLine->start->coords().x();

        const QString existingLegend =
            m_labelLineLegendMap.value(existingLine);

        const QString existingName =
            m_labelLineNameMap.value(existingLine);

        // Only reject the exact same landmark on the same channel.
        // Different landmarks are allowed to share the same timestamp.
        if (existingLegend == legendName &&
            existingName == labelName &&
            qAbs(existingX - x) < DUPLICATE_EPSILON)
        {
            return;
        }
    }

    QCPItemLine *labelLine = new QCPItemLine(m_plot);
    labelLine->setPen(QPen(Qt::black));
    labelLine->start->setCoords(x, m_plot->yAxis->range().lower);
    labelLine->end  ->setCoords(x, m_plot->yAxis->range().upper);
    labelLine->setSelectable(true);
    m_labelLines.append(labelLine);
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
    m_labelLineLegendMap[labelLine] = legendName;
    emit landmarkAdded(legendName, x, labelName);
    m_plot->replot();
}

void Label::removeLabelAt(double x, const QString &legendName)
{
    if (!m_plot || m_labelLines.isEmpty())
        return;

    // Local helpers (idempotent removal)
    auto plotHasItem = [this](QCPAbstractItem *it) -> bool {
        if (!m_plot || !it)
            return false;
        for (int i = 0, n = m_plot->itemCount(); i < n; ++i) {
            if (m_plot->item(i) == it)
                return true;
        }
        return false;
    };

    auto safeRemove = [&](QCPAbstractItem *it) {
        if (plotHasItem(it))
            m_plot->removeItem(it);
    };

    static const double EPS = 1e-6;

    // Find the exact matching line for this channel + offset
    auto matchIt = std::find_if(
        m_labelLines.begin(), m_labelLines.end(),
        [&](QCPItemLine *line) {
            if (!line)
                return false;

            const QString lineLegend = m_labelLineLegendMap.value(line);
            const double  lineX      = line->start->coords().x();

            return (lineLegend == legendName && std::fabs(lineX - x) < EPS);
        });

    if (matchIt == m_labelLines.end())
        return;

    QCPItemLine *line = *matchIt;
    const double lx   = line->start->coords().x();

    // Clear any selection/drag state tied to this line
    if (m_currentlySelectedLine == line)
        m_currentlySelectedLine = nullptr;

    if (m_draggedLine == line) {
        m_isDragging = false;
        m_draggedHandle = nullptr;
        m_draggedLine = nullptr;
    }

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
    if (QCPItemText *bottom = m_lineToBottomText.take(line)) {
        safeRemove(bottom);
        m_labelTexts.removeOne(bottom);
    }

    // Clean maps and remove the line
    m_labelLineNameMap.remove(line);
    m_labelLineLegendMap.remove(line);
    safeRemove(line);
    m_labelLines.erase(matchIt);

    emit landmarkRemoved(lx);

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

        static const double EPS = 1e-6;
        if (lineLegend == legendName && std::fabs(lineX - xOffset) < EPS) {
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
    const double EPS = 1e-6;
    for (QCPItemLine *line : m_labelLines) {
        const double lx = line->start->coords().x();
        if (std::abs(lx - x) < EPS) {
            m_labelLineNameMap[line] = actualName;
            return;
        }
    }
}

void Label::refreshGeometryForCurrentAxes()
{
    if (!m_plot)
        return;

    const double yBottom = m_plot->yAxis->range().lower;
    const double yTop    = m_plot->yAxis->range().upper;

    for (QCPItemLine *line : m_labelLines) {
        if (!line)
            continue;

        const double x = line->start->coords().x();

        // Stretch the line to the current visible Y range
        line->start->setCoords(x, yBottom);
        line->end->setCoords(x, yTop);

        // Reposition the bottom numeric text
        if (QCPItemText *bottomText = m_lineToBottomText.value(line, nullptr)) {
            bottomText->position->setCoords(x, yBottom);
        }

        // Reposition the top handle
        for (auto it = m_labelHandleMapText.begin(); it != m_labelHandleMapText.end(); ++it) {
            if (it.value() == line && it.key()) {
                it.key()->position->setCoords(x, yTop);
            }
        }
    }

    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

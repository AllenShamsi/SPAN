#include "KinematicVisualizer.h"

#include <QDebug>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QRegularExpression>

#include <algorithm>
#include <limits>
#include <cmath>

// ---------- Static members ----------
QList<QCustomPlot*> KinematicVisualizer::customPlots;
QCustomPlot* KinematicVisualizer::lastPlotWithLine = nullptr;

QCPItemRect* KinematicVisualizer::selectionRect = nullptr;

QHash<QCustomPlot*, QCPItemLine*> KinematicVisualizer::vLinesMap;
QHash<QCustomPlot*, QCPItemRect*> KinematicVisualizer::globalSelectionRects;
QHash<QCustomPlot*, QCPItemLine*> KinematicVisualizer::globalSelectionLeftLines;
QHash<QCustomPlot*, QCPItemLine*> KinematicVisualizer::globalSelectionRightLines;
QHash<QCustomPlot*, QCPItemText*> KinematicVisualizer::globalSelectionLeftLabels;
QHash<QCustomPlot*, QCPItemText*> KinematicVisualizer::globalSelectionRightLabels;
QHash<QCustomPlot*, QCPItemText*> KinematicVisualizer::globalSelectionDistanceLabels;

// ---------- color helpers (shading by axis) ----------
namespace {

// Axis shading: X darkest, Y base, Z lightest
inline QColor shadeForAxis(const QColor& base, const QString& axisKey) {
    if (axisKey.compare("X", Qt::CaseInsensitive) == 0) return base.darker(120);
    if (axisKey.compare("Y", Qt::CaseInsensitive) == 0) return base;
    if (axisKey.compare("Z", Qt::CaseInsensitive) == 0) return base.lighter(120);
    return base;
}

inline QPen makePenFromBase(const QColor& base, const QString& axisKey,
                            double width = 1.0, bool cosmetic = true)
{
    QPen p(shadeForAxis(base, axisKey));
    p.setWidthF(width);
    p.setCosmetic(cosmetic);
    return p;
}

} // namespace

// ---------- Construction / teardown ----------
KinematicVisualizer::KinematicVisualizer(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this)),
    m_label(new Label(customPlot))
{
    // Layout
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(customPlot);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    // Plot defaults
    customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);

    // Mouse / range signals
    connect(customPlot, &QCustomPlot::mousePress,   this, &KinematicVisualizer::onAnyMousePress);
    connect(customPlot, &QCustomPlot::mouseRelease, this, &KinematicVisualizer::onMouseRelease);

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
            this,              SLOT  (synchronizePlots(QCPRange)));

    connect(customPlot->xAxis,
            qOverload<const QCPRange &>(&QCPAxis::rangeChanged),
            this,
            [this](const QCPRange &range) {
                rescaleAudioYAxisToVisibleRange(range);
            });

    // Mouse tracking & event filter
    setMouseTracking(true);
    customPlot->setMouseTracking(true);
    customPlot->installEventFilter(this);

    // Register plot instance and setup cursor items
    customPlots.append(customPlot);
    setupCursorItems(customPlot);

    // Hide top x-axis by default
    customPlot->xAxis2->setVisible(false);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->xAxis2->setTicks(false);
    customPlot->xAxis2->setBasePen(Qt::NoPen);
    customPlot->xAxis2->setTickPen(Qt::NoPen);
    customPlot->xAxis2->setSubTickPen(Qt::NoPen);
}

KinematicVisualizer::~KinematicVisualizer() {
    customPlots.removeAll(customPlot);

    // Remove this plot's vertical line
    if (vLinesMap.contains(customPlot)) {
        if (auto *v = vLinesMap.take(customPlot)) {
            if (v->parentPlot()) v->parentPlot()->removeItem(v);
        }
    }

    // If shared selection rectangle belongs to this plot, remove it
    if (selectionRect && selectionRect->parentPlot() == customPlot) {
        selectionRect->parentPlot()->removeItem(selectionRect);
        selectionRect = nullptr;
    }
}


// ---------- Public API ----------
void KinematicVisualizer::visualizeSignal(const QMap<QString, QVector<double>> &dataMap,
                                          const QString &configName,
                                          double penWidth,
                                          int samplingRate)
{
    setupCustomPlot();
    customPlot->setFixedHeight(150);

    m_audioSignalFullRes.clear();
    m_autoScaleAudioY = false;

    // Legend
    customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    customPlot->legend->setIconSize(10, 10);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    // Data ranges for alignment and axes
    double maxTime   = 0.0;
    double globalMin = std::numeric_limits<double>::max();
    double globalMax = std::numeric_limits<double>::lowest();

    signalDataX.clear();
    signalDataY.clear();
    signalDataZ.clear();
    signalOffsets.clear();

    // First pass: global min/max
    for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        if (it->isEmpty()) continue;
        double localMin = *std::min_element(it->begin(), it->end());
        double localMax = *std::max_element(it->begin(), it->end());
        if (localMin < globalMin) globalMin = localMin;
        if (localMax > globalMax) globalMax = localMax;
    }
    double globalCenter = (globalMax + globalMin) / 2.0;

    // Second pass: build graphs
    for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        const QString key = it.key();               // "X", "Y", or "Z" (or "Audio" for audio visualizer)
        const auto   &vals = it.value();
        if (vals.isEmpty()) continue;

        QVector<double> x(vals.size());
        for (int i = 0; i < vals.size(); ++i)
            x[i] = static_cast<double>(i) / samplingRate;

        QCPGraph *graph = customPlot->addGraph();
        if (graph) {
            QPen pen;
            if (configName.compare("Audio", Qt::CaseInsensitive) == 0) {
                // Keep audio gray
                pen = QPen(QColor(140,140,140));
                pen.setWidthF(penWidth);
                pen.setCosmetic(true);
            } else {
                // Use base color provided by mainWindow (3D palette / derived mixing)
                QColor base = m_baseColor.isValid() ? m_baseColor
                                                    : QColor(127, 127, 127); // neutral fallback
                pen = makePenFromBase(base, key, penWidth, true);
            }
            graph->setPen(pen);
            graph->setBrush(Qt::NoBrush);
            graph->setLineStyle(QCPGraph::lsLine);

            // Avoid "Audio Audio" label duplication
            if (configName == key) graph->setName(configName);
            else                   graph->setName(configName);

            // Store original values for cursor readout
            QVector<double> originalYValues = vals;

            if (configName.compare("Audio", Qt::CaseInsensitive) == 0) {
                m_autoScaleAudioY = true;
                m_audioSignalFullRes.reserve(x.size());
                for (int i = 0; i < x.size(); ++i) {
                    m_audioSignalFullRes.append(qMakePair(x[i], originalYValues[i]));
                }
            }

            // Center around global center for stacked readability
            const double localMin = *std::min_element(vals.begin(), vals.end());
            const double localMax = *std::max_element(vals.begin(), vals.end());
            const double localCenter = (localMax + localMin) / 2.0;
            const double offset = globalCenter - localCenter;
            signalOffsets[key] = offset;

            QVector<double> yOffsetValues = vals;
            for (double &yv : yOffsetValues) yv += offset;

            QVector<double> plotX, plotY;

            if (configName.compare("Audio", Qt::CaseInsensitive) == 0) {
                // Keep full-resolution waveform data for audio
                plotX = x;
                plotY = yOffsetValues;
            } else {
                // Downsample non-audio traces to keep plotting responsive
                const int factor = qMax(1, yOffsetValues.size() / 10000);
                plotX.reserve((yOffsetValues.size() + factor - 1) / factor);
                plotY.reserve((yOffsetValues.size() + factor - 1) / factor);

                for (int i = 0; i < yOffsetValues.size(); i += factor) {
                    plotX.push_back(x[i]);
                    plotY.push_back(yOffsetValues[i]);
                }
            }

            graph->setData(plotX, plotY);

            // Populate for cursor interpolation
            if (key == "X") {
                signalDataX.reserve(x.size());
                for (int i = 0; i < x.size(); ++i) signalDataX.append(qMakePair(x[i], originalYValues[i]));
            } else if (key == "Y") {
                signalDataY.reserve(x.size());
                for (int i = 0; i < x.size(); ++i) signalDataY.append(qMakePair(x[i], originalYValues[i]));
            } else if (key == "Z") {
                signalDataZ.reserve(x.size());
                for (int i = 0; i < x.size(); ++i) signalDataZ.append(qMakePair(x[i], originalYValues[i]));
            }
        }

        if (!x.isEmpty() && x.last() > maxTime) maxTime = x.last();
    }

    // Axes ranges
    customPlot->xAxis->setRange(0, maxTime);
    xAxisMinLimit = 0;
    xAxisMaxLimit = maxTime;

    double padding = (globalMax - globalMin) * 0.1; // 10% padding
    if (padding == 0) padding = 1.0;
    customPlot->yAxis->setRange(globalMin - padding, globalMax + padding);

    if (m_autoScaleAudioY) {
        rescaleAudioYAxisToVisibleRange(customPlot->xAxis->range());
    }

    const bool isAudio = (configName.compare("Audio", Qt::CaseInsensitive) == 0);

    if (isAudio) {
        // Audio keeps the minimalist style + top time axis
        customPlot->xAxis2->setVisible(true);
        customPlot->xAxis2->setTickLabels(true);
        customPlot->xAxis2->setTicks(true);
        customPlot->xAxis2->setLabel("Time (s)");
        customPlot->xAxis2->setRange(0, maxTime);
        customPlot->xAxis2->setBasePen(QPen(Qt::black));
        customPlot->xAxis2->setTickPen(QPen(Qt::black));
        customPlot->xAxis2->setSubTickPen(QPen(Qt::black));
    } else {
        // ---- 1) Sparse Y-axis ticks for kinematic plots ----

        customPlot->yAxis->setTicks(true);
        customPlot->yAxis->setTickLabels(true);
        customPlot->yAxis->setSubTicks(false);

        QPen axisPen(QColor(160, 160, 160));
        customPlot->yAxis->setBasePen(axisPen);
        customPlot->yAxis->setTickPen(axisPen);
        customPlot->yAxis->setSubTickPen(Qt::NoPen);
        customPlot->yAxis->setTickLabelColor(QColor(110, 110, 110));

        QFont tickFont = font();
        tickFont.setPointSize(8);
        customPlot->yAxis->setTickLabelFont(tickFont);

        QSharedPointer<QCPAxisTicker> yTicker(new QCPAxisTicker);
        yTicker->setTickCount(4);   // sparse, readable
        customPlot->yAxis->setTicker(yTicker);

        // ---- 2) Subtle zero line ----
        QVector<double> zx(2), zy(2);
        zx[0] = 0.0;
        zx[1] = maxTime;
        zy[0] = 0.0;
        zy[1] = 0.0;

        QCPGraph *zeroGraph = customPlot->addGraph();
        if (zeroGraph) {
            QPen zeroPen(QColor(120, 120, 120, 140));
            zeroPen.setStyle(Qt::DashLine);
            zeroPen.setWidthF(1.0);
            zeroPen.setCosmetic(true);

            zeroGraph->setPen(zeroPen);
            zeroGraph->setLineStyle(QCPGraph::lsLine);
            zeroGraph->setBrush(Qt::NoBrush);
            zeroGraph->setData(zx, zy);
            zeroGraph->setName(QString());
            zeroGraph->removeFromLegend();
        }
    }

    customPlot->update();
}

void KinematicVisualizer::rescaleAudioYAxisToVisibleRange(const QCPRange &range)
{
    if (!m_autoScaleAudioY || m_audioSignalFullRes.isEmpty())
        return;

    const double left  = range.lower;
    const double right = range.upper;

    if (!(left < right))
        return;

    auto beginIt = std::lower_bound(
        m_audioSignalFullRes.begin(),
        m_audioSignalFullRes.end(),
        left,
        [](const QPair<double,double> &p, double xVal) {
            return p.first < xVal;
        });

    auto endIt = std::lower_bound(
        m_audioSignalFullRes.begin(),
        m_audioSignalFullRes.end(),
        right,
        [](const QPair<double,double> &p, double xVal) {
            return p.first < xVal;
        });

    if (beginIt == m_audioSignalFullRes.end() || beginIt == endIt)
        return;

    double localMin = std::numeric_limits<double>::max();
    double localMax = std::numeric_limits<double>::lowest();

    for (auto it = beginIt; it != endIt; ++it) {
        const double y = it->second;
        if (y < localMin) localMin = y;
        if (y > localMax) localMax = y;
    }

    if (localMin == std::numeric_limits<double>::max() ||
        localMax == std::numeric_limits<double>::lowest())
        return;

    double padding = (localMax - localMin) * 0.1;

    if (padding == 0.0) {
        padding = std::max(0.01, std::abs(localMax) * 0.1);
    }

    customPlot->yAxis->setRange(localMin - padding, localMax + padding);

    if (m_label) {
        m_label->refreshGeometryForCurrentAxes();
    } else {
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void KinematicVisualizer::visualizeSpectrogram(
    const QVector<QVector<double>> &spectrogramData,
    const QString & /*configName*/,
    double tStart,
    double tEnd,
    double maxFrequency,
    double dynamicRangeDb)
{
    const double duration = std::max(0.0, tEnd - tStart);

    // Did we already have a spectrogram and a meaningful x-range?
    const bool hadSpectrogram =
        m_isSpectrogram && (m_spectrogramDuration > 0.0);

    // Remember old x-range BEFORE resetting plot appearance
    QCPRange oldRange = customPlot->xAxis->range();

    // Preserve the current vertical cursor line state, if one is visible
    bool restoreVerticalCursor = false;
    double cursorX = 0.0;

    if (auto *vLine = vLinesMap.value(customPlot, nullptr)) {
        restoreVerticalCursor = vLine->visible();
        cursorX = vLine->start->coords().x();
    }

    // Reset plot appearance & plottables
    setupCustomPlot();

    // Mark this as a spectrogram and cache everything for later
    m_isSpectrogram         = true;
    m_spectrogramStartTime  = tStart;
    m_spectrogramDuration   = duration;
    m_spectrogramData       = spectrogramData;
    m_spectrogramDynRangeDb = dynamicRangeDb;

    customPlot->setProperty("isSpectrogram", true);

    const int nx = m_spectrogramData.size();
    const int ny = nx ? m_spectrogramData[0].size() : 0;

    auto *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    m_spectrogramColorMap = colorMap;

    colorMap->data()->setSize(nx, ny);
    colorMap->data()->setRange(QCPRange(tStart, tEnd),
                               QCPRange(0, maxFrequency)); // data coords (absolute time)

    // Copy data into the color map
    for (int ix = 0; ix < nx; ++ix)
        for (int iy = 0; iy < ny; ++iy)
            colorMap->data()->setCell(ix, iy, m_spectrogramData[ix][iy]);

    QCPColorGradient grad;
    grad.clearColorStops();
    grad.setColorInterpolation(QCPColorGradient::ciRGB);

    QCPColorGradient grayGradient;
    grayGradient.clearColorStops();
    grayGradient.setColorInterpolation(QCPColorGradient::ciRGB);
    grayGradient.setColorStopAt(0.0, Qt::white);
    grayGradient.setColorStopAt(1.0, Qt::black);
    colorMap->setGradient(grayGradient);

    const double specStart = m_spectrogramStartTime;
    const double specEnd   = m_spectrogramStartTime + m_spectrogramDuration;

    // Decide which x-range to use
    QCPRange xRangeToUse;
    if (hadSpectrogram) {
        // Keep current zoom, just clamp to [specStart, specEnd]
        double lower = qMax(specStart, oldRange.lower);
        double upper = qMin(specEnd,   oldRange.upper);
        if (!(lower < upper)) {
            lower = specStart;
            upper = specEnd;
        }
        xRangeToUse = QCPRange(lower, upper);
    } else {
        // First time: show the whole spectrogram range
        xRangeToUse = QCPRange(specStart, specEnd);
    }

    // Apply axes
    customPlot->xAxis->setRange(xRangeToUse);
    customPlot->yAxis->setRange(0, maxFrequency);

    // Initial color scale using CURRENT visible time window
    updateSpectrogramColorScaleForRange(xRangeToUse);

    // No legend entry
    colorMap->setName(QString());

    // Restore the vertical cursor line if it was visible before the rebuild
    if (restoreVerticalCursor) {
        if (auto *vLine = vLinesMap.value(customPlot, nullptr)) {
            vLine->start->setCoords(cursorX, customPlot->yAxis->range().lower);
            vLine->end->setCoords(cursorX, customPlot->yAxis->range().upper);
            vLine->setVisible(true);
        }
    }

    // Make sure it actually repaints now
    customPlot->replot(QCustomPlot::rpQueuedReplot);
}





void KinematicVisualizer::setTrackedParameter(const QString &parameter) {
    trackedParameter = parameter;
}

void KinematicVisualizer::setSignalData(const QMap<QString, QVector<double>> &dataMap) {
    signalDataX.clear();
    signalDataY.clear();
    signalDataZ.clear();

    if (dataMap.contains("X")) {
        const QVector<double> &xData = dataMap["X"];
        for (int i = 0; i < xData.size(); ++i)
            signalDataX.append(qMakePair(static_cast<double>(i), xData[i]));
    }
    if (dataMap.contains("Y")) {
        const QVector<double> &yData = dataMap["Y"];
        for (int i = 0; i < yData.size(); ++i)
            signalDataY.append(qMakePair(static_cast<double>(i), yData[i]));
    }
    if (dataMap.contains("Z")) {
        const QVector<double> &zData = dataMap["Z"];
        for (int i = 0; i < zData.size(); ++i)
            signalDataZ.append(qMakePair(static_cast<double>(i), zData[i]));
    }
}

void KinematicVisualizer::clearSelectionRect() {
    // Remove selection rectangles, boundary lines, and labels from every plot
    for (QCustomPlot* plot : customPlots) {
        if (globalSelectionRects.contains(plot))
            plot->removeItem(globalSelectionRects.value(plot));
        if (globalSelectionLeftLines.contains(plot))
            plot->removeItem(globalSelectionLeftLines.value(plot));
        if (globalSelectionRightLines.contains(plot))
            plot->removeItem(globalSelectionRightLines.value(plot));
        if (globalSelectionLeftLabels.contains(plot))
            plot->removeItem(globalSelectionLeftLabels.value(plot));
        if (globalSelectionRightLabels.contains(plot))
            plot->removeItem(globalSelectionRightLabels.value(plot));
        if (globalSelectionDistanceLabels.contains(plot))
            plot->removeItem(globalSelectionDistanceLabels.value(plot));

        plot->replot(QCustomPlot::rpQueuedReplot);
    }

    selectionRect = nullptr;

    globalSelectionRects.clear();
    globalSelectionLeftLines.clear();
    globalSelectionRightLines.clear();
    globalSelectionLeftLabels.clear();
    globalSelectionRightLabels.clear();
    globalSelectionDistanceLabels.clear();
}

QCPRange KinematicVisualizer::getSelectionRange() const {
    if (selectionRect) {
        const double lower = selectionRect->topLeft->coords().x();
        const double upper = selectionRect->bottomRight->coords().x();
        return QCPRange(lower, upper);
    }
    return QCPRange(0, 0); // invalid range
}


// ---------- Event plumbing ----------
bool KinematicVisualizer::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        auto *mouseEvent = static_cast<QMouseEvent*>(event);
        cursorPos = mouseEvent->pos();
        if (auto *plot = qobject_cast<QCustomPlot*>(object))
            updateCursorItems(plot);
        if (selecting) onMouseDrag(); // live update while dragging
        return true;
    } else if (event->type() == QEvent::Leave) {
        hideAllVerticalLines();
        hideHorizontalCursor();
        return true;
    } else if (event->type() == QEvent::Enter) {
        if (auto *plot = qobject_cast<QCustomPlot*>(object)) {
            updateCursorItems(plot);
            if (coordText)  coordText->setVisible(true);
            if (coordFrame) coordFrame->setVisible(true);
        }
        return true;
    }
    return QWidget::eventFilter(object, event);
}

void KinematicVisualizer::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

void KinematicVisualizer::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
}


// ---------- Cursor / selection visuals ----------
void KinematicVisualizer::setupCursorItems(QCustomPlot *plot) {
    if (!plot->layer("overlay"))
        plot->addLayer("overlay", plot->layer("main"), QCustomPlot::limAbove);
    if (!plot->layer("textOverlay"))
        plot->addLayer("textOverlay", plot->layer("overlay"), QCustomPlot::limAbove);

    // Per-plot vertical line (shared across instances via map)
    auto *vLine = new QCPItemLine(plot);
    vLine->setLayer("overlay");
    vLine->setPen(QPen(Qt::red, 1, Qt::SolidLine));
    vLine->start->setType(QCPItemPosition::ptPlotCoords);
    vLine->end->setType(QCPItemPosition::ptPlotCoords);
    vLine->setSelectable(false);
    vLine->setVisible(false);
    vLinesMap[plot] = vLine;

    // Only the "owner" plot (this instance) gets the horizontal cursor and text
    if (plot == customPlot) {
        hLine = new QCPItemLine(plot);
        hLine->setLayer("overlay");
        hLine->setPen(QPen(Qt::red, 1, Qt::SolidLine));
        hLine->start->setType(QCPItemPosition::ptPlotCoords);
        hLine->end->setType(QCPItemPosition::ptPlotCoords);
        hLine->setSelectable(false);
        hLine->setVisible(false);

        coordText  = new QCPItemText(plot);
        coordText->setLayer("textOverlay");

        // Use absolute (pixel) positioning
        coordText->position->setType(QCPItemPosition::ptAbsolute);

        // The position we give later will be the *top-left* corner of the text box
        coordText->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);

        // Inside that box, align text to the right so decimals line up
        coordText->setTextAlignment(Qt::AlignRight | Qt::AlignTop);

        coordText->setFont(QFont(font().family(), 10));
        coordText->setColor(Qt::black);
        coordText->setSelectable(false);
        coordText->setVisible(false);

        coordFrame = new QCPItemRect(plot);
        coordFrame->setLayer("overlay");
        coordFrame->setPen(QPen(Qt::NoPen));
        coordFrame->setBrush(QBrush(QColor(255, 0, 0, 50)));
        coordFrame->setVisible(false);
    }
}

void KinematicVisualizer::updateCursorItems(QCustomPlot *plot) {
    const double x = plot->xAxis->pixelToCoord(cursorPos.x());
    double y;
    QString usedAxis;   // Which axis we actually tracked (may be empty)

    if (plot->property("isSpectrogram").toBool()) {
        // Spectrogram: always track raw mouse Y
        y = plot->yAxis->pixelToCoord(cursorPos.y());
    } else {
        // Try to track data (honour radios + Auto)
        y = getYValueFromSignal(x, &usedAxis);

        // If no axis was chosen, use raw mouse Y instead
        if (usedAxis.isEmpty()) {
            y = plot->yAxis->pixelToCoord(cursorPos.y());
        }
    }

    // Keep vertical lines visible while the cursor is inside a plot.
    // We update their X position below for all plots, so there is no need
    // to hide and re-show them on every mouse move.

    // Main-plot horizontal cursor + text
    if (plot == customPlot) {
        double adjustedY = y;

        if (usedAxis == "X")      adjustedY += signalOffsets.value("X", 0.0);
        else if (usedAxis == "Y") adjustedY += signalOffsets.value("Y", 0.0);
        else if (usedAxis == "Z") adjustedY += signalOffsets.value("Z", 0.0);
        // If usedAxis is empty → Auto but multi-dim or user-picked axis without data:
        // we’re just using raw Y; no offset applied.

        if (cursorPos.y() >= 0 && cursorPos.y() <= plot->height()) {
            hLine->start->setCoords(plot->xAxis->range().lower, adjustedY);
            hLine->end->setCoords(plot->xAxis->range().upper, adjustedY);
            hLine->setVisible(true);
        } else {
            hLine->setVisible(false);
        }

        // Update text
        const QString coordStr = QString("X: %1\nY: %2").arg(x, 0, 'f', 3).arg(y, 0, 'f', 3);
        coordText->setText(coordStr);

        const QFontMetrics fm(coordText->font());

        // Measure the full two-line text
        const QRect textRect = fm.boundingRect(QRect(), 0, coordStr);
        const int padding = 5;
        const int frameWidth  = textRect.width()  + padding * 2;
        const int frameHeight = textRect.height() + padding * 2;

        // LEFT edge of the box is a bit to the right of the cursor
        const int leftX = cursorPos.x() + 20;
        const int topY  = cursorPos.y();

        // Frame: [leftX, topY - padding] → [leftX + frameWidth, topY + frameHeight - padding]
        coordFrame->topLeft->setPixelPosition(
            QPoint(leftX, topY - padding));
        coordFrame->bottomRight->setPixelPosition(
            QPoint(leftX + frameWidth, topY + frameHeight - padding));
        coordFrame->setVisible(true);

        // Text top-left goes inside the frame with padding
        coordText->position->setPixelPosition(
            QPoint(leftX + padding, topY));
        coordText->setVisible(true);

        customPlot->replot(QCustomPlot::rpQueuedReplot);
    }

    // Mirror the active X line across all plots
    updateVerticalLineInAllPlots(x);
}

void KinematicVisualizer::hideAllVerticalLines() {
    for (QCustomPlot *plot : customPlots) {
        if (auto *v = vLinesMap.value(plot, nullptr))
            v->setVisible(false);
        plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void KinematicVisualizer::hideHorizontalCursor() {
    if (hLine)      hLine->setVisible(false);
    if (coordText)  coordText->setVisible(false);
    if (coordFrame) coordFrame->setVisible(false);
    customPlot->replot(QCustomPlot::rpQueuedReplot);
}

void KinematicVisualizer::updateVerticalLineInAllPlots(double x)
{
    for (QCustomPlot *plot : customPlots) {
        if (auto *v = vLinesMap.value(plot, nullptr)) {
            v->start->setCoords(x, plot->yAxis->range().lower);
            v->end->setCoords(x, plot->yAxis->range().upper);
            v->setVisible(true);
            if (plot == customPlot)
                plot->replot(QCustomPlot::rpQueuedReplot);
            else
                plot->replot();
        }
    }
}

// ---------- Selection band logic ----------
void KinematicVisualizer::onAnyMousePress(QMouseEvent *event) {
    // If a label handle was pressed, handle that and skip selection logic
    const double pressX = customPlot->xAxis->pixelToCoord(event->pos().x());

    if (event->button() == Qt::RightButton &&
        (event->modifiers() & Qt::ShiftModifier))
    {
        selecting = false;
        clearSelectionRect();
        return;
    }

    if (QCPAbstractItem* clicked = customPlot->itemAt(event->pos(), true)) {
        if (auto *textItem = qobject_cast<QCPItemText*>(clicked)) {
            if (m_label && m_label->isHandle(textItem)) {
                m_clickedHandle = textItem;
                m_pressX = pressX;
                event->accept();
                return;
            }
        }
    }

    // Otherwise, (re)start selection flow
    clearSelectionRect();
    m_pressX   = pressX;
    m_pressPos = event->pos();
    selecting  = true;
}

void KinematicVisualizer::onMouseRelease() {
    // Finish label-handle move
    if (m_clickedHandle) {
        const double releaseX = customPlot->xAxis->pixelToCoord(cursorPos.x());
        const double deltaX   = releaseX - m_pressX;
        if (m_label) m_label->moveHandleBy(m_clickedHandle, deltaX);
        m_clickedHandle = nullptr;
        customPlot->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    // Finish selection
    selecting = false;
    if (selectionRect) {
        selectionRect->bottomRight->setCoords(
            customPlot->xAxis->pixelToCoord(cursorPos.x()),
            customPlot->yAxis->range().lower
            );
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    } else {
        clearSelectionRect();
    }
}

void KinematicVisualizer::onMouseDrag() {
    if (!selecting) return;

    // Lazy-create the selection rect only after a small drag threshold
    if (!selectionRect) {
        const int dragThreshold = 5;
        if ((cursorPos - m_pressPos).manhattanLength() < dragThreshold)
            return;

        // Keep the active plot rectangle pointer for existing logic
        selectionRect = new QCPItemRect(customPlot);
        selectionRect->setLayer("overlay");
        selectionRect->setPen(QPen(Qt::NoPen));
        selectionRect->setBrush(QBrush(QColor(255, 0, 0, 50)));
        selectionRect->topLeft->setType(QCPItemPosition::ptPlotCoords);
        selectionRect->bottomRight->setType(QCPItemPosition::ptPlotCoords);
        selectionRect->topLeft->setCoords(m_pressX, customPlot->yAxis->range().upper);
        selectionRect->bottomRight->setCoords(m_pressX, customPlot->yAxis->range().lower);
        globalSelectionRects[customPlot] = selectionRect;

        // Create rectangles, boundary lines, and labels in every plot now
        for (QCustomPlot* plot : customPlots) {
            if (plot != customPlot) {
                auto *rect = new QCPItemRect(plot);
                rect->setLayer("overlay");
                rect->setPen(QPen(Qt::NoPen));
                rect->setBrush(QBrush(QColor(255, 0, 0, 20))); // lighter alpha for other plots
                rect->topLeft->setType(QCPItemPosition::ptPlotCoords);
                rect->bottomRight->setType(QCPItemPosition::ptPlotCoords);
                rect->topLeft->setCoords(m_pressX, plot->yAxis->range().upper);
                rect->bottomRight->setCoords(m_pressX, plot->yAxis->range().lower);
                globalSelectionRects[plot] = rect;
            }

            // Left boundary
            auto *leftLine = new QCPItemLine(plot);
            leftLine->setLayer("overlay");
            leftLine->setPen(QPen(Qt::red, 1, Qt::DotLine));
            leftLine->start->setType(QCPItemPosition::ptPlotCoords);
            leftLine->end->setType(QCPItemPosition::ptPlotCoords);
            leftLine->start->setCoords(m_pressX, plot->yAxis->range().lower);
            leftLine->end->setCoords(m_pressX, plot->yAxis->range().upper);
            globalSelectionLeftLines[plot] = leftLine;

            // Right boundary
            auto *rightLine = new QCPItemLine(plot);
            rightLine->setLayer("overlay");
            rightLine->setPen(QPen(Qt::red, 1, Qt::DotLine));
            rightLine->start->setType(QCPItemPosition::ptPlotCoords);
            rightLine->end->setType(QCPItemPosition::ptPlotCoords);
            rightLine->start->setCoords(m_pressX, plot->yAxis->range().lower);
            rightLine->end->setCoords(m_pressX, plot->yAxis->range().upper);
            globalSelectionRightLines[plot] = rightLine;

            // Labels only on the active plot
            if (plot == customPlot) {
                auto *leftLabel = new QCPItemText(plot);
                leftLabel->setLayer("textOverlay");
                leftLabel->setFont(QFont("Arial", 10));
                leftLabel->setColor(Qt::darkRed);
                leftLabel->position->setType(QCPItemPosition::ptPlotCoords);
                leftLabel->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);
                leftLabel->position->setCoords(m_pressX, plot->yAxis->range().upper);
                leftLabel->setText(QString::number(m_pressX, 'f', 3));
                globalSelectionLeftLabels[plot] = leftLabel;

                auto *rightLabel = new QCPItemText(plot);
                rightLabel->setLayer("textOverlay");
                rightLabel->setFont(QFont("Arial", 10));
                rightLabel->setColor(Qt::darkRed);
                rightLabel->position->setType(QCPItemPosition::ptPlotCoords);
                rightLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
                rightLabel->position->setCoords(m_pressX, plot->yAxis->range().upper);
                rightLabel->setText(QString::number(m_pressX, 'f', 3));
                globalSelectionRightLabels[plot] = rightLabel;

                auto *distLabel = new QCPItemText(plot);
                distLabel->setLayer("textOverlay");
                distLabel->setFont(QFont("Arial", 10));
                distLabel->setColor(Qt::darkRed);
                distLabel->setClipToAxisRect(false);
                distLabel->position->setType(QCPItemPosition::ptPlotCoords);
                distLabel->setPositionAlignment(Qt::AlignCenter | Qt::AlignTop);
                const int topPixel = plot->axisRect()->top();
                const double topY = plot->yAxis->pixelToCoord(topPixel + 5);
                distLabel->position->setCoords(m_pressX, topY);
                distLabel->setText(QString::number(0.0, 'f', 3));
                globalSelectionDistanceLabels[plot] = distLabel;
            }
        }
    } else {
        // Update existing selection
        const double newRightX = customPlot->xAxis->pixelToCoord(cursorPos.x());
        selectionRect->bottomRight->setCoords(newRightX, customPlot->yAxis->range().lower);

        // Fixed top Y in plot coords for the active plot labels
        const int topPixel = customPlot->axisRect()->top();
        const double fixedTopYValues   = customPlot->yAxis->pixelToCoord(topPixel + 2);
        const double fixedTopYDistance = customPlot->yAxis->pixelToCoord(topPixel + 7);

        for (QCustomPlot* plot : customPlots) {
            const double leftX = selectionRect->topLeft->coords().x();

            if (globalSelectionRects.contains(plot)) {
                auto *rect = globalSelectionRects.value(plot);
                rect->topLeft->setCoords(leftX, plot->yAxis->range().upper);
                rect->bottomRight->setCoords(newRightX, plot->yAxis->range().lower);
            }

            if (globalSelectionLeftLines.contains(plot)) {
                auto *leftLine = globalSelectionLeftLines.value(plot);
                leftLine->start->setCoords(leftX,  plot->yAxis->range().lower);
                leftLine->end->setCoords  (leftX,  plot->yAxis->range().upper);
            }
            if (globalSelectionRightLines.contains(plot)) {
                auto *rightLine = globalSelectionRightLines.value(plot);
                rightLine->start->setCoords(newRightX, plot->yAxis->range().lower);
                rightLine->end->setCoords  (newRightX, plot->yAxis->range().upper);
            }
            if (plot == customPlot) {
                if (globalSelectionLeftLabels.contains(plot)) {
                    auto *leftLabel = globalSelectionLeftLabels.value(plot);
                    leftLabel->position->setCoords(leftX, fixedTopYValues);
                    leftLabel->setText(QString::number(leftX, 'f', 3));
                }
                if (globalSelectionRightLabels.contains(plot)) {
                    auto *rightLabel = globalSelectionRightLabels.value(plot);
                    rightLabel->position->setCoords(newRightX, fixedTopYValues);
                    rightLabel->setText(QString::number(newRightX, 'f', 3));
                }
                if (globalSelectionDistanceLabels.contains(plot)) {
                    auto *distLabel = globalSelectionDistanceLabels.value(plot);
                    const double centerX = (leftX + newRightX) / 2.0;
                    distLabel->position->setCoords(centerX, fixedTopYDistance);
                    const double distance = std::fabs(newRightX - leftX);
                    distLabel->setText(QString::number(distance, 'f', 3));

                    // Hide if label text won't fit between boundaries
                    const QFontMetrics fm(distLabel->font());
                    const int  textPixelW     = fm.boundingRect(distLabel->text()).width();
                    const double coordPerPix  = customPlot->xAxis->range().size() / customPlot->axisRect()->width();
                    const double textCoordW   = textPixelW * coordPerPix;
                    distLabel->setVisible(distance >= textCoordW);
                }
            }
            plot->replot(QCustomPlot::rpQueuedReplot);
        }
    }

    customPlot->replot(QCustomPlot::rpQueuedReplot);
}


// ---------- Syncing ----------
void KinematicVisualizer::synchronizePlots(const QCPRange &newRange)
{
    for (QCustomPlot *plot : customPlots) {
        if (plot == customPlot)
            continue;

        plot->blockSignals(true);
        plot->xAxis->setRange(newRange);
        plot->xAxis2->setRange(newRange);
        plot->blockSignals(false);

        plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void KinematicVisualizer::synchronizeYAxes() {
    if (customPlots.size() < 2) return;
    QCustomPlot *ref = customPlots.first();
    for (QCustomPlot *plot : customPlots) {
        if (plot == ref) continue;
        connect(ref->xAxis, SIGNAL(rangeChanged(QCPRange)), plot->xAxis, SLOT(setRange(QCPRange)));
        connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ref->xAxis, SLOT(setRange(QCPRange)));
        connect(ref->yAxis, SIGNAL(rangeChanged(QCPRange)), plot->yAxis, SLOT(setRange(QCPRange)));
        connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ref->yAxis, SLOT(setRange(QCPRange)));
    }
}


// ---------- Utilities ----------
void KinematicVisualizer::setupCustomPlot() {

    customPlot->clearPlottables();
    m_spectrogramColorMap = nullptr;

    // Hide bottom X ticks/labels; we'll show top axis when it's audio
    customPlot->xAxis->setTicks(false);
    customPlot->xAxis->setTickLabels(false);
    customPlot->xAxis->setBasePen(Qt::NoPen);
    customPlot->xAxis->setTickPen(Qt::NoPen);
    customPlot->xAxis->setSubTickPen(Qt::NoPen);

    // Hide Y ticks/labels; minimalist look
    customPlot->yAxis->setTicks(false);
    customPlot->yAxis->setTickLabels(false);

    customPlot->axisRect()->setAutoMargins(QCP::msTop | QCP::msRight | QCP::msBottom);
    customPlot->axisRect()->setMinimumMargins(QMargins(36, 0, 0, 0));

    // Performance tweak
    customPlot->setNotAntialiasedElements(QCP::aeAll);

    // Reset spectrogram property
    customPlot->setProperty("isSpectrogram", false);

    // Hide top axis by default
    customPlot->xAxis2->setVisible(false);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->xAxis2->setTicks(false);
    customPlot->xAxis2->setBasePen(Qt::NoPen);
    customPlot->xAxis2->setTickPen(Qt::NoPen);
    customPlot->xAxis2->setSubTickPen(Qt::NoPen);
}

double KinematicVisualizer::getSignalValueAt(double x, QString *usedAxis) const
{
    return const_cast<KinematicVisualizer*>(this)->getYValueFromSignal(x, usedAxis);
}

double KinematicVisualizer::getYValueFromSignal(double x, QString *usedAxis)
{
    auto interpolate = [](const QVector<QPair<double,double>> &sig, double xVal) -> double {
        if (sig.isEmpty()) return 0.0;

        auto it = std::lower_bound(sig.begin(), sig.end(), xVal,
                                   [](const QPair<double,double> &a, double v){
                                       return a.first < v;
                                   });
        if (it == sig.end())   return sig.last().second;
        if (it == sig.begin()) return sig.first().second;

        auto prev = std::prev(it);
        const double x1 = prev->first, y1 = prev->second;
        const double x2 = it->first,   y2 = it->second;
        const double dx = (x2 - x1);
        return dx == 0.0 ? y1 : (y1 + (y2 - y1) * (xVal - x1) / dx);
    };

    QString axis;

    // A) EXPLICIT MODE: X / Y / Z selected
    if (!trackedParameter.isEmpty()) {
        if (trackedParameter == "X" && !signalDataX.isEmpty())      axis = "X";
        else if (trackedParameter == "Y" && !signalDataY.isEmpty()) axis = "Y";
        else if (trackedParameter == "Z" && !signalDataZ.isEmpty()) axis = "Z";
        // If user picked an axis that has NO data in this plot,
        // we leave axis empty and will fall back to raw mouse Y.
    }
    // B) AUTO MODE: "Auto" radio is selected → trackedParameter == ""
    else {
        int count = 0;
        if (!signalDataX.isEmpty()) { axis = "X"; ++count; }
        if (!signalDataY.isEmpty()) {
            if (count == 0) axis = "Y";
            ++count;
        }
        if (!signalDataZ.isEmpty()) {
            if (count == 0) axis = "Z";
            ++count;
        }

        // If 0 or >1 axes have data → no single obvious axis, disable data-tracking
        if (count != 1)
            axis.clear();
    }

    if (usedAxis) *usedAxis = axis;

    if (axis == "X") return interpolate(signalDataX, x);
    if (axis == "Y") return interpolate(signalDataY, x);
    if (axis == "Z") return interpolate(signalDataZ, x);

    // No axis chosen → caller will treat this as "no tracking" and use raw mouse Y
    return 0.0;
}



void KinematicVisualizer::setZoomLimits(double minLimit, double maxLimit) {
    xAxisMinLimit = minLimit;
    xAxisMaxLimit = maxLimit;
}

bool KinematicVisualizer::isAtZoomOutLimit() {
    const QCPRange r = customPlot->xAxis->range();
    return (r.lower <= xAxisMinLimit && r.upper >= xAxisMaxLimit);
}

void KinematicVisualizer::zoomToSelection() {
    if (!selectionRect) return;
    const double xMin = selectionRect->topLeft->coords().x();
    const double xMax = selectionRect->bottomRight->coords().x();
    customPlot->xAxis->setRange(xMin, xMax);
    customPlot->update();
    clearSelectionRect();
}

void KinematicVisualizer::updateSpectrogramColorScaleForRange(const QCPRange &visibleRange)
{
    if (!m_isSpectrogram || !m_spectrogramColorMap)
        return;
    if (m_spectrogramData.isEmpty() || m_spectrogramDuration <= 0.0)
        return;

    const int nx = m_spectrogramData.size();
    const int ny = m_spectrogramData[0].size();
    if (nx <= 0 || ny <= 0)
        return;

    const double specStart = m_spectrogramStartTime;
    const double specEnd   = m_spectrogramStartTime + m_spectrogramDuration;

    // Intersect visible range with spectrogram range
    double a = qMax(specStart, visibleRange.lower);
    double b = qMin(specEnd,   visibleRange.upper);
    if (!(a < b)) {
        // Fallback to full spectrogram window
        a = specStart;
        b = specEnd;
    }

    // Map absolute time -> relative [0, duration] for index computation
    const double relA = a - specStart;
    const double relB = b - specStart;

    int ixMin = static_cast<int>(std::floor((relA / m_spectrogramDuration) * nx));
    int ixMax = static_cast<int>(std::ceil ((relB / m_spectrogramDuration) * nx)) - 1;

    ixMin = qBound(0, ixMin, nx - 1);
    ixMax = qBound(ixMin, ixMax, nx - 1);

    double dataMin = std::numeric_limits<double>::max();
    double dataMax = std::numeric_limits<double>::lowest();

    for (int ix = ixMin; ix <= ixMax; ++ix) {
        const QVector<double> &row = m_spectrogramData[ix];
        for (int iy = 0; iy < ny; ++iy) {
            const double v = row[iy];
            if (v < dataMin) dataMin = v;
            if (v > dataMax) dataMax = v;
        }
    }

    if (dataMin == std::numeric_limits<double>::max())
        return; // no data

    // Use the user-chosen dynamic range (in dB) around the local max.
    double dyn = (m_spectrogramDynRangeDb > 0.0) ? m_spectrogramDynRangeDb : 70.0;

    double upper = dataMax;
    double lower = upper - dyn;
    if (lower < dataMin)
        lower = dataMin;

    m_spectrogramColorMap->setDataRange(QCPRange(lower, upper));
    customPlot->replot(QCustomPlot::rpQueuedReplot);
}

#include "Sensor3DVisualizer.h"
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DCamera>
#include <QtDataVisualization/QTouch3DInputHandler>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QScatterDataProxy>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStackedLayout>

Sensor3DVisualizer::Sensor3DVisualizer(QWidget *parent)
    : QWidget(parent),
    scatterPlot(new Q3DScatter()),
    legendLayout(new QHBoxLayout()),
    titleLabel(new QLabel(this))
{
    initializeGraph();
}

Sensor3DVisualizer::~Sensor3DVisualizer()
{
}

void Sensor3DVisualizer::initializeGraph()
{
    auto *inputHandler = new QTouch3DInputHandler(scatterPlot);
    inputHandler->setZoomEnabled(true);        // pinch / wheel zoom
    inputHandler->setRotationEnabled(true);    // mouse drag rotation
    inputHandler->setSelectionEnabled(true);   // clicking points selects them
    inputHandler->setZoomAtTargetEnabled(true);

    scatterPlot->setActiveInputHandler(inputHandler);

    scatterPlot->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

    // Axis titles (your code)
    scatterPlot->axisX()->setTitle("X Axis");
    scatterPlot->axisY()->setTitle("Z Axis");
    scatterPlot->axisZ()->setTitle("Y Axis");

    scatterPlot->axisX()->setTitleVisible(false);
    scatterPlot->axisY()->setTitleVisible(false);
    scatterPlot->axisZ()->setTitleVisible(false);

    // --- Grid / tick granularity ---
    auto *xAxis = scatterPlot->axisX();
    auto *yAxis = scatterPlot->axisY();
    auto *zAxis = scatterPlot->axisZ();

    // Major ticks / grid lines
    xAxis->setSegmentCount(2);   // 4–7 depending on your range
    yAxis->setSegmentCount(2);
    zAxis->setSegmentCount(2);

    // Label formatting
    xAxis->setLabelFormat(QStringLiteral("%.0f"));
    yAxis->setLabelFormat(QStringLiteral("%.0f"));
    zAxis->setLabelFormat(QStringLiteral("%.0f"));

    // Keep your fixed Y range helper
    setYaxisBoundaries(-100, 100);

    auto *camera = scatterPlot->scene()->activeCamera();

    // Initial viewpoint
    camera->setCameraPreset(Q3DCamera::CameraPresetIsometricRightHigh);

    // Zoom limits
    camera->setMinZoomLevel(50.0f);   // cannot zoom further out than this
    camera->setMaxZoomLevel(250.0f);  // cannot zoom further in than this
    camera->setZoomLevel(120.0f);     // starting zoom

    // Optional: prevent camera from flipping upside down
    camera->setWrapYRotation(false);
    camera->setWrapXRotation(false);

}


void Sensor3DVisualizer::setSensorColors(const QMap<int, QColor>& colors)
{
    sensorColors = colors;
}

void Sensor3DVisualizer::updateData(const std::vector<channel>& data)
{
    // Clear previous series
    while (scatterPlot->seriesList().size() > 0) {
        QScatter3DSeries *series = scatterPlot->seriesList().at(0);
        scatterPlot->removeSeries(series);
        delete series;
    }

    QMap<int, QScatterDataArray*> dataArrayMap;

    for (size_t ch = 0; ch < data.size(); ++ch) {
        for (const auto& sample : data[ch].samples) {

            // Skip zero points
            if (sample.x == 0 && sample.y == 0 && sample.z == 0) {
                continue;
            }

            if (!dataArrayMap.contains(static_cast<int>(ch))) {
                dataArrayMap[static_cast<int>(ch)] = new QScatterDataArray;
            }

            QScatterDataItem item;
            item.setPosition(QVector3D(sample.x, sample.z, sample.y));
            dataArrayMap[static_cast<int>(ch)]->append(item);
        }
    }

    for (auto it = dataArrayMap.begin(); it != dataArrayMap.end(); ++it) {
        QScatterDataProxy *proxy = new QScatterDataProxy;
        proxy->resetArray(it.value());

        QScatter3DSeries *series = new QScatter3DSeries(proxy);
        int channelIndex = it.key();
        if (channelIndex < data.size()) {
            series->setBaseColor(data[channelIndex].color);
        } else {
            series->setBaseColor(Qt::white);
        }

        // --- Point look ---
        series->setMesh(QAbstract3DSeries::MeshSphere);   // MeshCube, MeshPyramid, MeshMinimal, ...
        series->setItemSize(0.15f);

        // Optional: labels when hovered/selected
        series->setItemLabelFormat(QStringLiteral("@seriesName\nx: @xLabel\ny: @zLabel\nz: @yLabel"));
        series->setItemLabelVisible(false);  // set true if you want labels always on

        scatterPlot->addSeries(series);
    }

}

void Sensor3DVisualizer::createLegend(const std::vector<channel>& data)
{
    // Clear previous legend
    QLayoutItem *child;
    while ((child = legendLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // Set the vertical spacing to 0
    legendLayout->setSpacing(2);

    // Create legend items
    for (const auto& ch : data) {
        if (ch.sensorName != "Unknown") {
            QLabel *colorLabel = new QLabel;
            colorLabel->setFixedSize(10, 10);
            colorLabel->setStyleSheet(QString("background-color: %1").arg(ch.color.name()));

            QLabel *textLabel = new QLabel(ch.sensorName);
            QFont font = textLabel->font();
            font.setPointSize(9); // Set font size to 14
            textLabel->setFont(font);

            QHBoxLayout *legendItemLayout = new QHBoxLayout;
            legendItemLayout->addWidget(colorLabel);
            legendItemLayout->addWidget(textLabel);

            // Set the spacing between legend elements
            legendItemLayout->setSpacing(2);

            QWidget *legendItemWidget = new QWidget;
            legendItemWidget->setLayout(legendItemLayout);

            legendLayout->addWidget(legendItemWidget);
        }
    }
}


void Sensor3DVisualizer::setupScatterPlot(QVBoxLayout *hl3D,
                                          QWidget *plotTitleFrame,
                                          QWidget *plotLegendWidget,
                                          const QSize &minSize)
{
    if (!hl3D) return;

    hl3D->setSpacing(0);

    // --- 1) 3D scatter container ---
    QWidget *container = QWidget::createWindowContainer(scatterPlot);

    // Use layout-driven sizing:
    // - Expanding policy so it grows with its parent layout
    // - Optional minimum size so it’s not tiny
    if (minSize.isValid()) {
        container->setMinimumSize(minSize);
    }
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    hl3D->addWidget(container);

    // --- 2) Optional title ---
    if (plotTitleFrame) {
        titleLabel->setAlignment(Qt::AlignLeft);
        titleLabel->setStyleSheet("font-size: 14px; color: black;");

        QVBoxLayout *titleLayout = new QVBoxLayout(plotTitleFrame);
        titleLayout->setSpacing(0);
        titleLayout->setContentsMargins(0, 0, 0, 0);
        titleLayout->addWidget(titleLabel);

        plotTitleFrame->setFixedHeight(50);
    }

    // --- 3) Optional legend ---
    if (plotLegendWidget) {
        QWidget *legendContainer = new QWidget;
        legendContainer->setLayout(legendLayout);
        legendContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        QVBoxLayout *legendWidgetLayout = new QVBoxLayout(plotLegendWidget);
        legendWidgetLayout->setSpacing(2);
        legendWidgetLayout->setContentsMargins(0, 0, 0, 0);
        legendWidgetLayout->addWidget(legendContainer);
    }
}



void Sensor3DVisualizer::setGraphTitle(const QString &title)
{
    titleLabel->setText(title);
}

void Sensor3DVisualizer::setYaxisBoundaries(float min, float max)
{
    scatterPlot->axisY()->setRange(min, max);
}

void Sensor3DVisualizer::resetAxisBoundaries()
{
    scatterPlot->axisX()->setAutoAdjustRange(true);
    scatterPlot->axisY()->setAutoAdjustRange(true);
    scatterPlot->axisZ()->setAutoAdjustRange(true);
}

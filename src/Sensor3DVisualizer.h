#ifndef SENSOR3DVISUALIZER_H
#define SENSOR3DVISUALIZER_H

#include <QWidget>
#include <QMap>
#include <QColor>
#include <QSize>
#include <vector>

#include "SensorData.h"   // for channel

class Q3DScatter;
class QHBoxLayout;
class QLabel;
class QVBoxLayout;

class Sensor3DVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit Sensor3DVisualizer(QWidget *parent = nullptr);
    ~Sensor3DVisualizer() override;

    // Build the 3D scatter in the given layout.
    // minSize is optional; if invalid -> it will just expand with the layout.
    void setupScatterPlot(QVBoxLayout *hl3D,
                          QWidget *plotTitleFrame,
                          QWidget *plotLegendWidget,
                          const QSize &minSize = QSize());

    // Optional: set per-sensor colors if you ever use this externally
    void setSensorColors(const QMap<int, QColor> &colors);

    void setGraphTitle(const QString &title);

    void setYaxisBoundaries(float min, float max);
    void resetAxisBoundaries();

    // Main API for updating data + legend
    void updateData(const std::vector<channel> &data);
    void createLegend(const std::vector<channel> &data);

private:
    void initializeGraph();

    Q3DScatter        *scatterPlot;
    QHBoxLayout       *legendLayout;
    QLabel            *titleLabel;

    QMap<int, QColor> sensorColors;
};

#endif // SENSOR3DVISUALIZER_H

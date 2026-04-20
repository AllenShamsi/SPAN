#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <vector>
#include <QString>
#include <QColor>
#include <cstddef>

struct sampleData {
    float x, y, z, phi, theta, rms, extra;

    static constexpr std::size_t num_members = 7;

    sampleData(float xVal = 0.0f,
               float yVal = 0.0f,
               float zVal = 0.0f,
               float phiVal = 0.0f,
               float thetaVal = 0.0f,
               float rmsVal = 0.0f,
               float extraVal = 0.0f)
        : x(xVal),
        y(yVal),
        z(zVal),
        phi(phiVal),
        theta(thetaVal),
        rms(rmsVal),
        extra(extraVal) {}
};

struct channel {
    int sensorNumber = -1;
    QString sensorName;
    QColor color;
    std::vector<sampleData> samples;
};

#endif // SENSORDATA_H

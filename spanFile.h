#ifndef SPANFILE_H
#define SPANFILE_H

#include <vector>
#include <string>
#include <map>
#include <QString>

enum class motionParam {
    Displacement,
    Velocity,
    Acceleration
};

struct SensorConfig {
    QString name;
    bool x;
    bool y;
    bool z;
    motionParam param;

    SensorConfig(const QString& name = "",
                 bool x = true,
                 bool y = true,
                 bool z = true,
                 motionParam param = motionParam::Displacement)
        : name(name), x(x), y(y), z(z), param(param) {}
};

struct posData {
    float x, y, z, phi, theta, rms;

    posData(float x = 0.0f,
            float y = 0.0f,
            float z = 0.0f,
            float phi = 0.0f,
            float theta = 0.0f,
            float rms = 0.0f)
        : x(x), y(y), z(z), phi(phi), theta(theta), rms(rms) {}
};

struct spanFile {
    std::string path;

    int wavSR;
    int posSR;
    int numChannels;

    std::map<int, std::string> sensors;

    std::vector<float> audioData;
    std::vector<std::vector<posData>> channelsData;

    std::vector<SensorConfig> configurations;

    spanFile(int wavSR = 0, int posSR = 0, int numChannels = 0)
        : wavSR(wavSR), posSR(posSR), numChannels(numChannels) {}
};

#endif // SPANFILE_H

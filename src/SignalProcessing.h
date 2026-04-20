#ifndef SIGNALPROCESSING_H
#define SIGNALPROCESSING_H

#include <vector>
#include "spanFile.h"

namespace SignalProcessing {

std::vector<posData> interpolatePositionData(const std::vector<posData>& v,
                                             int posSR,
                                             int wavSR);

std::vector<posData> subtractComponent(const std::vector<posData>& a,
                                       const std::vector<posData>& b);

std::vector<posData> computeEuclideanDistance(const std::vector<posData>& s1,
                                              const std::vector<posData>& s2);

std::vector<posData> calculateVelocity(const std::vector<posData> &positionData,
                                       int samplingRate);

std::vector<posData> calculateAcceleration(const std::vector<posData> &velocityData,
                                           int samplingRate);

static constexpr int    kSpecDefaultMaxFrequencyHz = 5000;
static constexpr double kSpecDefaultWindowMs       = 5.0;
static constexpr double kSpecDefaultOverlapRatio   = 0.5;
static constexpr double kSpecDefaultDynRangeDb     = 60.0;

// spectrogram
std::vector<std::vector<double>> computeSpectrogram(
    const std::vector<float>& audioData,
    int samplingRate,
    int maxFrequency    = kSpecDefaultMaxFrequencyHz,
    double windowMs     = kSpecDefaultWindowMs,
    double overlapRatio = kSpecDefaultOverlapRatio);

} // namespace SignalProcessing

#endif // SIGNALPROCESSING_H

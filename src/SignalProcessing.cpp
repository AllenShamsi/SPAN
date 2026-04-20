#include "SignalProcessing.h"

#include <algorithm>
#include <fftw3.h>
#include <cmath>

namespace {

// Moving average over x/y/z with prefix sums.
std::vector<posData> applyMovingAverageFilter(const std::vector<posData>& data,
                                              int windowSize)
{
    if (data.empty() || windowSize <= 1) return data;

    const int n = static_cast<int>(data.size());
    windowSize = std::max(2, std::min(windowSize, n));
    if ((windowSize & 1) == 0) ++windowSize;   // prefer odd window
    const int half = windowSize / 2;

    std::vector<double> px(n + 1, 0.0), py(n + 1, 0.0), pz(n + 1, 0.0);
    for (int i = 0; i < n; ++i) {
        px[i + 1] = px[i] + data[i].x;
        py[i + 1] = py[i] + data[i].y;
        pz[i + 1] = pz[i] + data[i].z;
    }

    std::vector<posData> out(n);
    for (int i = 0; i < n; ++i) {
        const int L   = std::max(0, i - half);
        const int R   = std::min(n - 1, i + half);
        const int cnt = R - L + 1;

        out[i]   = data[i]; // keep phi/theta/rms
        out[i].x = (px[R + 1] - px[L]) / cnt;
        out[i].y = (py[R + 1] - py[L]) / cnt;
        out[i].z = (pz[R + 1] - pz[L]) / cnt;
    }

    return out;
}

// High-order central difference gradient over x/y/z.
std::vector<posData> computeGradient(const std::vector<posData> &data, double dt)
{
    std::vector<posData> gradient(data.size());
    if (data.size() < 4) return gradient;

    for (size_t i = 1; i < data.size() - 1; ++i) {
        if (i == 1 || i == data.size() - 2) {
            gradient[i].x = (data[i + 1].x - data[i - 1].x) / (2.0 * dt);
            gradient[i].y = (data[i + 1].y - data[i - 1].y) / (2.0 * dt);
            gradient[i].z = (data[i + 1].z - data[i - 1].z) / (2.0 * dt);
        } else {
            gradient[i].x = (-data[i + 2].x + 8.0 * data[i + 1].x
                             - 8.0 * data[i - 1].x + data[i - 2].x) / (12.0 * dt);
            gradient[i].y = (-data[i + 2].y + 8.0 * data[i + 1].y
                             - 8.0 * data[i - 1].y + data[i - 2].y) / (12.0 * dt);
            gradient[i].z = (-data[i + 2].z + 8.0 * data[i + 1].z
                             - 8.0 * data[i - 1].z + data[i - 2].z) / (12.0 * dt);
        }
    }

    if (data.size() >= 2) {
        gradient[0].x = (data[1].x - data[0].x) / dt;
        gradient[0].y = (data[1].y - data[0].y) / dt;
        gradient[0].z = (data[1].z - data[0].z) / dt;

        const size_t last = data.size() - 1;
        gradient[last].x = (data[last].x - data[last - 1].x) / dt;
        gradient[last].y = (data[last].y - data[last - 1].y) / dt;
        gradient[last].z = (data[last].z - data[last - 1].z) / dt;
    }

    return gradient;
}

} // namespace

namespace SignalProcessing {

std::vector<posData> interpolatePositionData(const std::vector<posData>& v,
                                             int posSR,
                                             int wavSR)
{
    std::vector<posData> up;
    if (posSR <= 0 || wavSR <= 0 || v.empty()) return up;

    const double factor = static_cast<double>(wavSR) / posSR;
    const int    newSize = static_cast<int>(std::llround(v.size() * factor));
    if (newSize <= 0) return up;

    up.resize(newSize);
    for (int i = 0; i < newSize; ++i) {
        const double t   = i / factor;
        const int    idx = static_cast<int>(std::floor(t));
        const double a   = t - idx;

        if (idx >= static_cast<int>(v.size()) - 1) {
            up[i] = v.back();
        } else {
            const auto& s0 = v[idx];
            const auto& s1 = v[idx + 1];
            up[i].x     = s0.x     * (1 - a) + s1.x     * a;
            up[i].y     = s0.y     * (1 - a) + s1.y     * a;
            up[i].z     = s0.z     * (1 - a) + s1.z     * a;
            up[i].phi   = s0.phi   * (1 - a) + s1.phi   * a;
            up[i].theta = s0.theta * (1 - a) + s1.theta * a;
            up[i].rms   = s0.rms   * (1 - a) + s1.rms   * a;
        }
    }
    return up;
}

std::vector<posData> subtractComponent(const std::vector<posData>& a,
                                       const std::vector<posData>& b)
{
    std::vector<posData> out;
    if (a.size() != b.size()) return out;

    out.reserve(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        posData d{};
        d.x     = a[i].x     - b[i].x;
        d.y     = a[i].y     - b[i].y;
        d.z     = a[i].z     - b[i].z;
        d.phi   = a[i].phi   - b[i].phi;
        d.theta = a[i].theta - b[i].theta;
        d.rms   = a[i].rms   - b[i].rms;
        out.push_back(d);
    }
    return out;
}

std::vector<posData> computeEuclideanDistance(const std::vector<posData>& s1,
                                              const std::vector<posData>& s2)
{
    std::vector<posData> out;
    if (s1.size() != s2.size()) return out;

    out.reserve(s1.size());
    for (size_t i = 0; i < s1.size(); ++i) {
        posData d{};
        const double dist = std::sqrt(std::pow(s1[i].x - s2[i].x, 2) +
                                      std::pow(s1[i].y - s2[i].y, 2) +
                                      std::pow(s1[i].z - s2[i].z, 2));
        d.x = dist;
        d.y = 0;
        d.z = 0;
        d.phi = 0;
        d.theta = 0;
        d.rms = 0;
        out.push_back(d);
    }
    return out;
}

std::vector<posData> calculateVelocity(const std::vector<posData> &positionData,
                                       int samplingRate)
{
    std::vector<posData> velocityData(positionData.size());
    if (positionData.size() > 1 && samplingRate > 0) {
        const double dt = 1.0 / samplingRate;
        velocityData    = computeGradient(positionData, dt);
        velocityData    = applyMovingAverageFilter(velocityData, 1000);
    }
    return velocityData;
}

std::vector<posData> calculateAcceleration(const std::vector<posData> &velocityData,
                                           int samplingRate)
{
    std::vector<posData> accelerationData(velocityData.size());
    if (velocityData.size() > 1 && samplingRate > 0) {
        const double dt = 1.0 / samplingRate;
        auto smoothedV  = applyMovingAverageFilter(velocityData, 50);
        accelerationData = computeGradient(smoothedV, dt);
        accelerationData = applyMovingAverageFilter(accelerationData, 800);
    }
    return accelerationData;
}

// -------- Spectrogram helpers (FFT) --------

static std::vector<double> generateGaussianWindow(int size)
{
    std::vector<double> window(size);
    const double alpha = 2.5;
    const int    half  = size / 2;
    const double sigma = half / alpha;
    const double sigma2 = 2.0 * sigma * sigma;

    for (int i = 0; i < size; ++i) {
        double n = i - half;
        window[i] = std::exp(-n * n / sigma2);
    }
    return window;
}

static std::vector<double> computeMagnitude(const fftw_complex* out, int size)
{
    std::vector<double> magnitudes(size);
    for (int i = 0; i < size; ++i) {
        const double re = out[i][0];
        const double im = out[i][1];
        const double mag = std::sqrt(re*re + im*im);
        // log-scaled magnitude, like your original
        magnitudes[i] = 10.0 * std::log10(mag + 1.0);
    }
    return magnitudes;
}

static std::vector<double> performFFT(const std::vector<double>& frame,
                                      const std::vector<double>& window,
                                      int maxFrequencyIndex)
{
    const int N = static_cast<int>(frame.size());
    if (N <= 0) return {};

    fftw_complex* in  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < N; ++i) {
        in[i][0] = frame[i] * window[i];
        in[i][1] = 0.0;
    }

    fftw_execute(p);
    std::vector<double> magnitudes = computeMagnitude(out, maxFrequencyIndex);

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    return magnitudes;
}

std::vector<std::vector<double>>
computeSpectrogram(const std::vector<float>& audioData,
                   int samplingRate,
                   int maxFrequency,
                   double windowMs,
                   double overlapRatio)
{
    std::vector<std::vector<double>> spectrogram;

    if (samplingRate <= 0 || audioData.empty())
        return spectrogram;

    // frame / hop sizes
    const int windowSize = static_cast<int>(samplingRate * (windowMs * 1e-3)); // ms -> s
    if (windowSize <= 0) return spectrogram;

    if (overlapRatio < 0.0) overlapRatio = 0.0;
    if (overlapRatio >= 1.0) overlapRatio = 0.99;
    const int overlap  = static_cast<int>(windowSize * overlapRatio);
    const int stepSize = windowSize - overlap;
    if (stepSize <= 0) return spectrogram;

    const int N = static_cast<int>(audioData.size());
    if (N < windowSize) return spectrogram;

    const int nyquistBin = windowSize / 2;
    const int maxFrequencyIndex = std::min(
        nyquistBin,
        static_cast<int>(std::floor(double(maxFrequency) * windowSize / samplingRate))
        );

    const int numWindows = 1 + (N - windowSize) / stepSize;
    if (numWindows <= 0) return spectrogram;

    spectrogram.reserve(numWindows);

    const std::vector<double> gaussianWindow = generateGaussianWindow(windowSize);

    for (int w = 0; w < numWindows; ++w) {
        const int start = w * stepSize;

        std::vector<double> frame(windowSize);
        for (int j = 0; j < windowSize; ++j) {
            frame[j] = static_cast<double>(audioData[start + j]);
        }

        spectrogram.push_back(
            performFFT(frame, gaussianWindow, maxFrequencyIndex)
            );
    }

    return spectrogram;
}


} // namespace SignalProcessing

#include "UtilityFunctions.h"

#include <stdexcept>
#include <cmath>
#include <set>

namespace {

constexpr double kMinNorm = 1e-10;
constexpr double kMinCrossNorm = 1e-8;

bool sampleHasValidXYZ(const sampleData& s)
{
    return !std::isnan(s.x) && !std::isnan(s.y) && !std::isnan(s.z);
}

Eigen::Vector3d safeNormalized(const Eigen::Vector3d& v, const char* what)
{
    const double n = v.norm();
    if (n < kMinNorm) {
        throw std::runtime_error(std::string("Cannot normalize vector for: ") + what);
    }
    return v / n;
}

void validateChannelIndices(const std::vector<channel>& data,
                            const std::vector<int>& indices,
                            const char* label)
{
    if (indices.empty()) {
        throw std::runtime_error(std::string("No indices provided for ") + label);
    }

    std::set<int> uniqueIdx;
    for (int idx : indices) {
        if (idx < 0 || idx >= static_cast<int>(data.size())) {
            throw std::runtime_error(std::string("Out-of-range channel index for ") + label);
        }
        uniqueIdx.insert(idx);
    }

    if (uniqueIdx.size() != indices.size()) {
        throw std::runtime_error(std::string("Duplicate channel index found for ") + label);
    }
}

void validateConsistentSampleCounts(const std::vector<channel>& data)
{
    if (data.empty()) {
        throw std::runtime_error("No channel data provided.");
    }

    const size_t nSamples = data.front().samples.size();
    if (nSamples == 0) {
        throw std::runtime_error("Channel data contain zero samples.");
    }

    for (const auto& ch : data) {
        if (ch.samples.size() != nSamples) {
            throw std::runtime_error("Inconsistent sample counts across channels.");
        }
    }
}

bool referenceFrameIsValid(const std::vector<channel>& data,
                           const std::vector<int>& refIdx,
                           int frameIndex)
{
    for (int idx : refIdx) {
        if (!sampleHasValidXYZ(data[idx].samples[frameIndex])) {
            return false;
        }
    }
    return true;
}

bool pointSetIsNonDegenerate(const Eigen::MatrixXd& pts)
{
    if (pts.rows() < 3)
        return false;

    const Eigen::Vector3d p0 = pts.row(0).transpose();
    const Eigen::Vector3d p1 = pts.row(1).transpose();
    const Eigen::Vector3d p2 = pts.row(2).transpose();

    const Eigen::Vector3d v1 = p1 - p0;
    const Eigen::Vector3d v2 = p2 - p0;

    return v1.cross(v2).norm() >= kMinCrossNorm;
}

} // namespace

Eigen::Vector3d computeMeanSensorPosition(const channel& sensor)
{
    Eigen::Vector3d mean = Eigen::Vector3d::Zero();
    int count = 0;

    for (const auto& sample : sensor.samples) {
        if (!sampleHasValidXYZ(sample))
            continue;

        mean(0) += sample.x;
        mean(1) += sample.y;
        mean(2) += sample.z;
        ++count;
    }

    if (count == 0) {
        throw std::runtime_error("No valid XYZ samples found for sensor.");
    }

    return mean / static_cast<double>(count);
}

Eigen::MatrixXd extractReferencePointsForFrame(const std::vector<channel>& data,
                                               const std::vector<int>& refIdx,
                                               int frameIndex)
{
    Eigen::MatrixXd pts(refIdx.size(), 3);

    for (size_t i = 0; i < refIdx.size(); ++i) {
        const auto& sample = data[refIdx[i]].samples[frameIndex];
        pts(i, 0) = sample.x;
        pts(i, 1) = sample.y;
        pts(i, 2) = sample.z;
    }

    return pts;
}

Eigen::MatrixXd transformPointsToCanonicalFrame(const Eigen::MatrixXd& pts,
                                                const Eigen::Matrix3d& axes,
                                                const Eigen::Vector3d& origin)
{
    Eigen::MatrixXd out(pts.rows(), 3);

    for (int i = 0; i < pts.rows(); ++i) {
        const Eigen::Vector3d p = pts.row(i).transpose();
        const Eigen::Vector3d shifted = p - origin;
        const Eigen::Vector3d canonical = axes.transpose() * shifted;
        out.row(i) = canonical.transpose();
    }

    return out;
}

Eigen::Matrix4d estimateRigidTransform(const Eigen::MatrixXd& sourcePts,
                                       const Eigen::MatrixXd& targetPts)
{
    if (sourcePts.rows() != targetPts.rows() || sourcePts.cols() != 3 || targetPts.cols() != 3) {
        throw std::runtime_error("Rigid transform requires matched [n x 3] point sets.");
    }

    if (sourcePts.rows() < 3) {
        throw std::runtime_error("Rigid transform requires at least 3 matched points.");
    }

    if (!pointSetIsNonDegenerate(sourcePts) || !pointSetIsNonDegenerate(targetPts)) {
        throw std::runtime_error("Degenerate point configuration in rigid transform.");
    }

    const Eigen::Vector3d sourceMean = sourcePts.colwise().mean();
    const Eigen::Vector3d targetMean = targetPts.colwise().mean();

    const Eigen::MatrixXd sourceCentered = sourcePts.rowwise() - sourceMean.transpose();
    const Eigen::MatrixXd targetCentered = targetPts.rowwise() - targetMean.transpose();

    const Eigen::Matrix3d H = sourceCentered.transpose() * targetCentered;
    Eigen::JacobiSVD<Eigen::Matrix3d> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);

    Eigen::Matrix3d R = svd.matrixV() * svd.matrixU().transpose();

    if (R.determinant() < 0) {
        Eigen::Matrix3d V = svd.matrixV();
        V.col(2) *= -1.0;
        R = V * svd.matrixU().transpose();
    }

    const Eigen::Vector3d t = targetMean - R * sourceMean;

    Eigen::Matrix4d RT = Eigen::Matrix4d::Identity();
    RT.block<3, 3>(0, 0) = R.transpose();
    RT.block<1, 3>(3, 0) = t.transpose();

    return RT;
}

OcclusalCorrectionModel buildOcclusalCorrectionModel(const std::vector<channel>& bitePlaneData,
                                                     const std::vector<int>& refIdx,
                                                     const std::vector<int>& bpIdx)
{
    validateConsistentSampleCounts(bitePlaneData);
    validateChannelIndices(bitePlaneData, refIdx, "reference sensors");
    validateChannelIndices(bitePlaneData, bpIdx, "bite-plane sensors");

    if (bpIdx.size() != 3) {
        throw std::runtime_error("Exactly three bite-plane sensors are required.");
    }

    if (refIdx.size() < 3) {
        throw std::runtime_error("At least three reference sensors are required.");
    }

    const Eigen::Vector3d bpL = computeMeanSensorPosition(bitePlaneData[bpIdx[0]]);
    const Eigen::Vector3d bpF = computeMeanSensorPosition(bitePlaneData[bpIdx[1]]);
    const Eigen::Vector3d bpR = computeMeanSensorPosition(bitePlaneData[bpIdx[2]]);

    const Eigen::Vector3d origin = (bpL + bpF + bpR) / 3.0;
    const Eigen::Vector3d lrMid = 0.5 * (bpL + bpR);

    const Eigen::Vector3d xAxis = safeNormalized(bpR - bpL, "bite-plane left-right axis");
    const Eigen::Vector3d ySeed = safeNormalized(bpF - lrMid, "bite-plane anterior axis");

    const Eigen::Vector3d rawZ = xAxis.cross(ySeed);
    if (rawZ.norm() < kMinCrossNorm) {
        throw std::runtime_error("Bite-plane sensors are nearly collinear.");
    }

    const Eigen::Vector3d zAxis = safeNormalized(rawZ, "bite-plane normal axis");
    const Eigen::Vector3d yAxis = safeNormalized(zAxis.cross(xAxis), "orthogonal anterior axis");

    Eigen::Matrix3d axes;
    axes.col(0) = xAxis;
    axes.col(1) = yAxis;
    axes.col(2) = zAxis;

    Eigen::MatrixXd meanRefPts(refIdx.size(), 3);
    for (size_t i = 0; i < refIdx.size(); ++i) {
        const Eigen::Vector3d meanRef = computeMeanSensorPosition(bitePlaneData[refIdx[i]]);
        meanRefPts.row(i) = meanRef.transpose();
    }

    const Eigen::MatrixXd canonicalRefPts =
        transformPointsToCanonicalFrame(meanRefPts, axes, origin);

    if (!pointSetIsNonDegenerate(canonicalRefPts)) {
        throw std::runtime_error("Canonical reference template is degenerate.");
    }

    OcclusalCorrectionModel model;
    model.origin = origin;
    model.axes = axes;
    model.canonicalReferencePts = canonicalRefPts;
    return model;
}

std::vector<channel> applyOcclusalCorrection(const OcclusalCorrectionModel& model,
                                             const std::vector<channel>& rawData,
                                             const std::vector<int>& refIdx)
{
    validateConsistentSampleCounts(rawData);
    validateChannelIndices(rawData, refIdx, "reference sensors");

    if (refIdx.size() < 3) {
        throw std::runtime_error("At least three reference sensors are required.");
    }

    std::vector<channel> corrected = rawData;
    const int nFrames = static_cast<int>(rawData[0].samples.size());

    for (int frame = 0; frame < nFrames; ++frame) {
        if (!referenceFrameIsValid(rawData, refIdx, frame)) {
            for (auto& ch : corrected) {
                ch.samples[frame].x = NAN;
                ch.samples[frame].y = NAN;
                ch.samples[frame].z = NAN;
                ch.samples[frame].phi = NAN;
                ch.samples[frame].theta = NAN;
            }
            continue;
        }

        const Eigen::MatrixXd currentRefPts = extractReferencePointsForFrame(rawData, refIdx, frame);

        if (!pointSetIsNonDegenerate(currentRefPts)) {
            for (auto& ch : corrected) {
                ch.samples[frame].x = NAN;
                ch.samples[frame].y = NAN;
                ch.samples[frame].z = NAN;
                ch.samples[frame].phi = NAN;
                ch.samples[frame].theta = NAN;
            }
            continue;
        }

        const Eigen::Matrix4d RT = estimateRigidTransform(currentRefPts, model.canonicalReferencePts);

        for (size_t ch = 0; ch < rawData.size(); ++ch) {
            const auto& s = rawData[ch].samples[frame];

            if (!sampleHasValidXYZ(s)) {
                corrected[ch].samples[frame].x = NAN;
                corrected[ch].samples[frame].y = NAN;
                corrected[ch].samples[frame].z = NAN;
                corrected[ch].samples[frame].phi = NAN;
                corrected[ch].samples[frame].theta = NAN;
                continue;
            }

            Eigen::RowVector4d p;
            p << s.x, s.y, s.z, 1.0;

            const Eigen::RowVector4d correctedP = p * RT;

            corrected[ch].samples[frame].x = correctedP(0);
            corrected[ch].samples[frame].y = correctedP(1);
            corrected[ch].samples[frame].z = correctedP(2);

            // Intentionally unchanged for now
            corrected[ch].samples[frame].phi = s.phi;
            corrected[ch].samples[frame].theta = s.theta;
        }
    }

    return corrected;
}

#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H

#include "SensorData.h"

#include <Eigen/Dense>
#include <vector>

struct OcclusalCorrectionModel
{
    Eigen::Vector3d origin;                 // Centroid of BP-L / BP-F / BP-R
    Eigen::Matrix3d axes;                   // Columns = canonical x, y, z axes
    Eigen::MatrixXd canonicalReferencePts;  // [nRef x 3]
};

Eigen::Vector3d computeMeanSensorPosition(const channel& sensor);
Eigen::MatrixXd extractReferencePointsForFrame(const std::vector<channel>& data,
                                               const std::vector<int>& refIdx,
                                               int frameIndex);
Eigen::MatrixXd transformPointsToCanonicalFrame(const Eigen::MatrixXd& pts,
                                                const Eigen::Matrix3d& axes,
                                                const Eigen::Vector3d& origin);

Eigen::Matrix4d estimateRigidTransform(const Eigen::MatrixXd& sourcePts,
                                       const Eigen::MatrixXd& targetPts);

OcclusalCorrectionModel buildOcclusalCorrectionModel(const std::vector<channel>& bitePlaneData,
                                                     const std::vector<int>& refIdx,
                                                     const std::vector<int>& bpIdx);

std::vector<channel> applyOcclusalCorrection(const OcclusalCorrectionModel& model,
                                             const std::vector<channel>& rawData,
                                             const std::vector<int>& refIdx);

#endif // UTILITYFUNCTIONS_H

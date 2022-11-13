#pragma once

#include "pinocchio/multibody/geometry.hpp"
#include <Eigen/Dense>

namespace placo {
/**
 * @brief Interpolate between two frames
 * @param frameA Frame A
 * @param frameB Frame B
 * @param AtoB A real number from 0 to 1 that controls the interpolation (0:
 * frame A, 1: frameB)
 * @return
 */
Eigen::Affine3d interpolate_frames(Eigen::Affine3d frameA,
                                   Eigen::Affine3d frameB, double AtoB);

/**
 * @brief Computes the "yaw" of an orientation
 * @param rotation the orientation
 * @return a scalar angle [rad]
 */
double frame_yaw(Eigen::Matrix3d rotation);

/**
 * @brief Makes an Affine3d from a 4x4 matrix (for python bindings)
 * @param matrix the 4x4 matrix
 * @return The Affine3d
 */
Eigen::Affine3d frame(Eigen::Matrix4d matrix);

/**
 * @brief Takes a 3D transformation and ensure it is "flat" on the floor
 * (setting z to 0 and keeping only yaw)
 * @param transformation a 3D transformation
 * @return a 3D transformation that lies on the floor (no pitch/roll and no z)
 */
Eigen::Affine3d flatten_on_floor(const Eigen::Affine3d &transformation);

/**
 * @brief Converts a pinocchio's SE3 transformation to Eigen Affine3d
 * @param se3 pinocchio SE3 transformation
 * @return Eigen affine3d
 */
Eigen::Affine3d pin_se3_to_eigen(const pinocchio::GeometryData::SE3 &se3);

/**
 * @brief Returns the acos of v, with safety on v values
 * @param v value
 */
double safe_acos(double v);
} // namespace placo

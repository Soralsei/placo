#pragma once

#include "placo/dynamics/task.h"
#include "placo/model/robot_wrapper.h"
#include "placo/tools/axises_mask.h"

namespace placo::dynamics
{
class RelativeOrientationTask : public Task
{
public:
  RelativeOrientationTask(RobotWrapper::FrameIndex frame_a_index, RobotWrapper::FrameIndex frame_b_index,
                          Eigen::Matrix3d R_a_b);

  /**
   * @brief Frame A
   */
  RobotWrapper::FrameIndex frame_a_index;

  /**
   * @brief Frame B
   */
  RobotWrapper::FrameIndex frame_b_index;

  /**
   * @brief Target relative orientation
   */
  Eigen::Matrix3d R_a_b;

  /**
   * @brief Target relative angular velocity
   */
  Eigen::Vector3d omega_a_b = Eigen::Vector3d::Zero();

  void update() override;
  std::string type_name() override;
  std::string error_unit() override;

  /**
   * @brief Mask
   */
  AxisesMask mask;
};
}  // namespace placo::dynamics
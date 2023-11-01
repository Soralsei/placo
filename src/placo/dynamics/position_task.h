#pragma once

#include "placo/dynamics/task.h"
#include "placo/model/robot_wrapper.h"
#include "placo/tools/axises_mask.h"

namespace placo::dynamics
{
class PositionTask : public Task
{
public:
  /**
   * @brief see \ref placo::dynamics::DynamicSolver::add_position_task
   */
  PositionTask(RobotWrapper::FrameIndex frame_index, Eigen::Vector3d target_world);

  /**
   * @brief Frame
   */
  RobotWrapper::FrameIndex frame_index;

  /**
   * @brief Target position in the world
   */
  Eigen::Vector3d target_world;

  /**
   * @brief Target velocity in the world
   */
  Eigen::Vector3d dtarget_world = Eigen::Vector3d::Zero();

  void update() override;
  std::string type_name() override;
  std::string error_unit() override;

  /**
   * @brief Mask
   */
  AxisesMask mask;
};
}  // namespace placo::dynamics
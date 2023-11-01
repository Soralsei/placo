#pragma once

#include "placo/kinematics/task.h"
#include "placo/tools/axises_mask.h"

namespace placo::kinematics
{
class KinematicsSolver;
struct OrientationTask : public Task
{
  /**
   * @brief See \ref KinematicsSolver::add_orientation_task
   */
  OrientationTask(RobotWrapper::FrameIndex frame_index, Eigen::Matrix3d R_world_frame);

  /**
   * @brief Frame
   */
  RobotWrapper::FrameIndex frame_index;

  /**
   * @brief Target frame orientation in the world
   */
  Eigen::Matrix3d R_world_frame;

  virtual void update();
  virtual std::string type_name();
  virtual std::string error_unit();

  /**
   * @brief Mask
   */
  AxisesMask mask;
};
}  // namespace placo::kinematics
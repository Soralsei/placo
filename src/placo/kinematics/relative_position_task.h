#pragma once

#include "placo/kinematics/task.h"
#include "placo/tools/axises_mask.h"

namespace placo::kinematics
{
class KinematicsSolver;
struct RelativePositionTask : public Task
{
  RelativePositionTask(RobotWrapper::FrameIndex frame_a, RobotWrapper::FrameIndex frame_b, Eigen::Vector3d target);

  RobotWrapper::FrameIndex frame_a;
  RobotWrapper::FrameIndex frame_b;
  Eigen::Vector3d target;

  virtual void update();
  virtual std::string type_name();
  virtual std::string error_unit();

  AxisesMask mask;
};
}  // namespace placo::kinematics
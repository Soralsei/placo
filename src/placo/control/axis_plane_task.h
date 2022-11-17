#pragma once

#include "placo/control/task.h"

namespace placo
{
class KinematicsSolver;
struct AxisPlaneTask : public Task
{
  AxisPlaneTask(RobotWrapper::FrameIndex frame_index, Eigen::Vector3d axis_frame, Eigen::Vector3d normal_world);

  RobotWrapper::FrameIndex frame_index;
  Eigen::Vector3d axis_frame;
  Eigen::Vector3d normal_world;

  virtual void update();
  virtual std::string type_name();
  virtual std::string error_unit();
};
}  // namespace placo
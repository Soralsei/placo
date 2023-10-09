#include "placo/control/position_task.h"
#include "placo/control/kinematics_solver.h"

namespace placo
{
PositionTask::PositionTask(RobotWrapper::FrameIndex frame_index, Eigen::Vector3d target_world)
  : frame_index(frame_index), target_world(target_world)
{
}

void PositionTask::update()
{
  auto T_world_frame = solver->robot->get_T_world_frame(frame_index);
  Eigen::Vector3d error = target_world - T_world_frame.translation();
  Eigen::MatrixXd J = solver->robot->frame_jacobian(frame_index, pinocchio::LOCAL_WORLD_ALIGNED);

  A = J.block(0, 0, 3, solver->N)(mask.indices, Eigen::placeholders::all);
  b = error(mask.indices, Eigen::placeholders::all);
}

std::string PositionTask::type_name()
{
  return "position";
}

std::string PositionTask::error_unit()
{
  return "m";
}
}  // namespace placo
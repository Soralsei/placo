#pragma once

#include "placo/kinematics/kinematics_solver.h"
#include "placo/planning/walk_pattern_generator.h"

namespace placo
{
class WalkTasks
{
public:
  void initialize_tasks(placo::kinematics::KinematicsSolver* solver, placo::model::HumanoidRobot* robot);
  void remove_tasks();
  virtual ~WalkTasks();

  void update_tasks(WalkPatternGenerator::Trajectory& trajectory, double t);
  void update_tasks(Eigen::Affine3d T_world_left, Eigen::Affine3d T_world_right, Eigen::Vector3d com_world,
                    Eigen::Matrix3d R_world_trunk);

  std::map<std::string, Eigen::Vector3d> get_tasks_error();

  placo::kinematics::KinematicsSolver* solver = nullptr;
  placo::model::HumanoidRobot* robot = nullptr;

  placo::kinematics::FrameTask left_foot_task;
  placo::kinematics::FrameTask right_foot_task;
  placo::kinematics::OrientationTask* trunk_orientation_task;

  placo::kinematics::CoMTask* com_task = nullptr;
  placo::kinematics::PositionTask* trunk_task = nullptr;

  void update_com_task();

  void reach_initial_pose(Eigen::Affine3d T_world_left, double feet_spacing, double com_height, double trunk_pitch);

  bool scaled = false;

  bool trunk_mode = false;
  double com_delay = 0.;
  double com_x = 0.;
  double com_y = 0.;
};
}  // namespace placo
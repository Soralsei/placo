#pragma once

#include "placo/footsteps/footsteps_planner.h"
#include "placo/model/humanoid_robot.h"
#include "placo/model/humanoid_parameters.h"
#include "placo/trajectory/cubic_spline_3d.h"
#include "placo/trajectory/swing_foot.h"
#include "placo/control/kinematics_solver.h"
#include "placo/control/frame_task.h"
#include "placo/control/com_task.h"
#include "placo/control/orientation_task.h"
#include "placo/problem/integrator.h"
#include "placo/problem/problem.h"
#include "placo/planning/lipm.h"

namespace placo
{
class WalkPatternGenerator
{
public:
  struct TrajectoryPart
  {
    SwingFoot::Trajectory swing_trajectory;
    double t_start;
    double t_end;

    FootstepsPlanner::Support support;
  };

  struct Trajectory
  {
    Trajectory();

    double com_height;
    double trunk_pitch;

    // Planned supports
    std::vector<FootstepsPlanner::Support> supports;

    // A part is the support and the swing trajectory
    std::vector<TrajectoryPart> parts;

    // CoM trajectory
    LIPM::Trajectory com;

    // Feet trajectory
    placo::CubicSpline left_foot_yaw;
    placo::CubicSpline right_foot_yaw;
    placo::CubicSpline trunk_yaw;

    placo::CubicSpline& yaw(HumanoidRobot::Side side);

    Eigen::Affine3d get_T_world_left(double t);
    Eigen::Affine3d get_T_world_right(double t);
    Eigen::Vector3d get_v_world_left(double t);
    Eigen::Vector3d get_v_world_right(double t);

    Eigen::Vector3d get_p_world_CoM(double t);
    Eigen::Matrix3d get_R_world_trunk(double t);

    HumanoidRobot::Side support_side(double t);
    bool is_both_support(double t);
    bool is_flying(HumanoidRobot::Side side, double t);

    FootstepsPlanner::Support get_support(double t);
    FootstepsPlanner::Support get_next_support(double t);
    FootstepsPlanner::Support get_prev_support(double t);

    /**
     * @brief Returns the trajectory time start for the support corresponding to the given time
     */
    double get_part_t_start(double t);

    // Trajectory duration
    double t_start = 0.0;
    double t_end = 0.0;

    // Number of dt planned by the jerk planner
    int jerk_planner_timesteps = 0;
  };

  WalkPatternGenerator(HumanoidRobot& robot, HumanoidParameters& parameters);

  /**
   * @brief Plan a walk trajectory following given footsteps based on the parameters of the WPG
   * @param supports Supports generated from the foosteps to follow
   * @return Planned trajectory
   */
  Trajectory plan(std::vector<FootstepsPlanner::Support>& supports, double t_start = 0.);

  /**
   * @brief Update the walk trajectory to follow given footsteps based on the parameters of the WPG.
   * It ensure a continuous CoM trajectory and replan only if replan_frequency dt have passed
   * @param supports Supports generated from the current foosteps or the new
   * ones to follow. Contain the current support
   * @param old_trajectory Current walk trajectory
   * @param t_replan The time (in the original trajectory) where the replan happens
   * @return True if the trajectory have been replanned, false it hasn't
   */
  Trajectory replan(std::vector<FootstepsPlanner::Support>& supports, Trajectory& old_trajectory, double t_replan);

  /**
   * @brief Checks if a trajectory can be replanned for supports
   */
  bool can_replan_supports(Trajectory& trajectory, double t_replan);

  /**
   * @brief Replan the supports for a given trajectory given a footsteps planner
   */
  std::vector<FootstepsPlanner::Support> replan_supports(FootstepsPlanner& planner, Trajectory& trajectory,
                                                         double t_replan);

protected:
  // Robot associated to the WPG
  HumanoidRobot& robot;

  // The parameters to use for planning. The values are forwarded to the relevant solvers when needed.
  HumanoidParameters& parameters;

  void planCoM(Trajectory& trajectory, Eigen::Vector2d initial_pos = Eigen::Vector2d::Zero(),
               Eigen::Vector2d initial_vel = Eigen::Vector2d::Zero(),
               Eigen::Vector2d initial_acc = Eigen::Vector2d::Zero(), Trajectory* old_trajectory = nullptr,
               double t_replan = 0.);

  void planFeetTrajectories(Trajectory& trajectory, Trajectory* old_trajectory = nullptr, double t_replan = 0.);

  int support_timesteps(FootstepsPlanner::Support& support);
};
}  // namespace placo
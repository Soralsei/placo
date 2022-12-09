#pragma once

#include "placo/model/robot_wrapper.h"

namespace placo
{
class HumanoidRobot : public RobotWrapper
{
public:
  /**
   * @brief Which side the foot is
   */
  enum Side
  {
    Left = 0,
    Right
  };

  static Side string_to_side(const std::string &str);
  static Side other_side(Side side);

  HumanoidRobot(std::string model_directory = "robot/");

  /**
   * @brief Updates which frame should be the current support
   */
  void update_support_side(Side side);
  void update_support_side(const std::string& side);
  void swap_support_side();
  void ensure_on_floor();

  Eigen::Affine3d get_T_world_left();
  Eigen::Affine3d get_T_world_right();
  Eigen::Affine3d get_T_world_trunk();

  // We suppose we have one support frame and associated transformation
  RobotWrapper::FrameIndex support_frame();
  RobotWrapper::FrameIndex flying_frame();

  /**
   * @brief The current side (left foot or right foot) supporting the robot
   */
  Side support_side;

  /**
   * @brief Transformation from support to world
   */
  Eigen::Affine3d T_world_support;

  // Useful frames
  RobotWrapper::FrameIndex left_foot;
  RobotWrapper::FrameIndex right_foot;
  RobotWrapper::FrameIndex trunk;
};
}  // namespace placo
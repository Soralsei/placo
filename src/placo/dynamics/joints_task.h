#pragma once

#include <map>
#include <string>
#include "placo/dynamics/task.h"
#include "placo/model/robot_wrapper.h"
#include "placo/tools/axises_mask.h"

namespace placo::dynamics
{
class JointsTask : public Task
{
public:
  /**
   * @brief see \ref placo::dynamics::DynamicSolver::add_gear_task
   */
  JointsTask();

  /**
   * @brief Maps joint to position targets
   */
  std::map<std::string, double> joints;

  /**
   * @brief Maps joints to velocity targets
   */
  std::map<std::string, double> djoints;

  /**
   * @brief Sets the target for a given joint
   * @param joint joint name
   * @param target target position
   * @param velocity target velocity
   */
  void set_joint(std::string joint, double target, double velocity = 0.);

  void update() override;
  std::string type_name() override;
  std::string error_unit() override;
};
}  // namespace placo::dynamics
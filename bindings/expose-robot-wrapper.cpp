#include <pinocchio/fwd.hpp>

#include "expose-utils.hpp"
#include "module.h"
#include "registry.h"
#include "placo/model/robot_wrapper.h"
#include "placo/model/humanoid_robot.h"
#include "placo/kinematics/kinematics_solver.h"
#include <Eigen/Dense>
#include <boost/python.hpp>

using namespace boost::python;
using namespace placo;

#ifdef HAVE_RHOBAN_UTILS
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(read_from_histories_overloads, read_from_histories, 2, 4);
#endif

template <class RobotType, class W1>
void exposeRobotType(class_<RobotType, W1>& type)
{
  type.add_property("state", &RobotType::state)
      .add_property("model", &RobotType::model)
      .add_property("collision_model", &RobotType::collision_model)
      .add_property("visual_model", &RobotType::visual_model)
      .def("load_collisions_pairs", &RobotType::load_collisions_pairs)
      .def("get_joint_offset", &RobotType::get_joint_offset)
      .def("get_joint_v_offset", &RobotType::get_joint_v_offset)
      .def("reset", &RobotType::reset)
      .def("neutral_state", &RobotType::neutral_state)
      .def("set_joint", &RobotType::set_joint)
      .def("get_joint", &RobotType::get_joint)
      .def("set_joint_velocity", &RobotType::set_joint_velocity)
      .def("get_joint_velocity", &RobotType::get_joint_velocity)
      .def("set_velocity_limit", &RobotType::set_velocity_limit)
      .def("set_velocity_limits", &RobotType::set_velocity_limits)
      .def("set_torque_limit", &RobotType::set_torque_limit)
      .def("set_joint_limits", &RobotType::set_joint_limits)
      .def("update_kinematics", &RobotType::update_kinematics)
      .def("get_T_world_fbase", &RobotType::get_T_world_fbase)
      .def("set_T_world_fbase", &RobotType::set_T_world_fbase)
      .def("com_world", &RobotType::com_world)
      .def("joint_names", &RobotType::joint_names)
      .def("actuated_joint_names", &RobotType::actuated_joint_names)
      .def("frame_names", &RobotType::frame_names)
      .def("self_collisions", &RobotType::self_collisions)
      .def("distances", &RobotType::distances)
      .def("com_jacobian", &RobotType::com_jacobian)
      .def("com_jacobian_time_variation", &RobotType::com_jacobian_time_variation)
      .def("generalized_gravity", &RobotType::generalized_gravity)
      .def("non_linear_effects", &RobotType::non_linear_effects)
      .def("mass_matrix", &RobotType::mass_matrix)
      .def("total_mass", &RobotType::total_mass)
      .def("integrate", &RobotType::integrate)
      .def(
          "static_gravity_compensation_torques",
          +[](RobotType& robot, const std::string& frame) { return robot.static_gravity_compensation_torques(frame); })
      .def(
          "static_gravity_compensation_torques_dict",
          +[](RobotType& robot, const std::string& frame) {
            auto torques = robot.static_gravity_compensation_torques(frame);
            boost::python::dict dict;

            for (auto& dof : robot.actuated_joint_names())
            {
              dict[dof] = torques[robot.get_joint_v_offset(dof)];
            }

            return dict;
          })
      .def(
          "torques_from_acceleration_with_fixed_frame",
          +[](RobotType& robot, Eigen::VectorXd qdd_a, const std::string& frame) {
            return robot.torques_from_acceleration_with_fixed_frame(qdd_a, frame);
          })
      .def(
          "torques_from_acceleration_with_fixed_frame_dict",
          +[](RobotType& robot, Eigen::VectorXd qdd_a, const std::string& frame) {
            auto torques = robot.torques_from_acceleration_with_fixed_frame(qdd_a, frame);
            boost::python::dict dict;

            for (auto& dof : robot.actuated_joint_names())
            {
              dict[dof] = torques[robot.get_joint_v_offset(dof) - 6];
            }

            return dict;
          })
      .def(
          "get_T_world_frame",
          +[](RobotType& robot, const std::string& frame) { return robot.get_T_world_frame(frame); })
      .def(
          "get_T_a_b", +[](RobotType& robot, const std::string& frameA,
                           const std::string& frameB) { return robot.get_T_a_b(frameA, frameB); })
      .def(
          "set_T_world_frame", +[](RobotType& robot, const std::string& frame,
                                   Eigen::Affine3d T_world_frame) { robot.set_T_world_frame(frame, T_world_frame); })
      .def(
          "frame_jacobian", +[](RobotType& robot, const std::string& frame,
                                const std::string& reference) { return robot.frame_jacobian(frame, reference); })
      .def(
          "frame_jacobian_time_variation",
          +[](RobotType& robot, const std::string& frame, const std::string& reference) {
            return robot.frame_jacobian_time_variation(frame, reference);
          })
      .def(
          "joint_jacobian", +[](RobotType& robot, const std::string& joint,
                                const std::string& reference) { return robot.joint_jacobian(joint, reference); })
      .def(
          "make_solver", +[](RobotType& robot) { return placo::kinematics::KinematicsSolver(robot); });
}

void exposeRobotWrapper()
{
  enum_<RobotWrapper::Flags>("Flags")
      .value("collision_as_visual", RobotWrapper::Flags::COLLISION_AS_VISUAL)
      .value("ignore_collisions", RobotWrapper::Flags::IGNORE_COLLISIONS);

  class__<RobotWrapper::State>("RobotWrapper_State")
      .add_property(
          "q", +[](const RobotWrapper::State& state) { return state.q; },
          +[](RobotWrapper::State& state, const Eigen::VectorXd& q) { state.q = q; })
      .add_property(
          "qd", +[](const RobotWrapper::State& state) { return state.qd; },
          +[](RobotWrapper::State& state, const Eigen::VectorXd& qd) { state.qd = qd; })
      .add_property(
          "qdd", +[](const RobotWrapper::State& state) { return state.qdd; },
          +[](RobotWrapper::State& state, const Eigen::VectorXd& qdd) { state.qdd = qdd; });
  ;

  class__<RobotWrapper::Collision>("Collision")
      .add_property("objA", &RobotWrapper::Collision::objA)
      .add_property("objB", &RobotWrapper::Collision::objB)
      .add_property("bodyA", &RobotWrapper::Collision::bodyA)
      .add_property("bodyB", &RobotWrapper::Collision::bodyB)
      .add_property("parentA", &RobotWrapper::Collision::parentA)
      .add_property("parentB", &RobotWrapper::Collision::parentB)
      .def(
          "get_contact", +[](RobotWrapper::Collision& collision, int index) { return collision.contacts[index]; });

  class__<RobotWrapper::Distance>("Distance")
      .add_property("objA", &RobotWrapper::Distance::objA)
      .add_property("objB", &RobotWrapper::Distance::objB)
      .add_property("parentA", &RobotWrapper::Distance::parentA)
      .add_property("parentB", &RobotWrapper::Distance::parentB)
      .add_property(
          "pointA", +[](RobotWrapper::Distance& distance) { return distance.pointA; })
      .add_property(
          "pointB", +[](RobotWrapper::Distance& distance) { return distance.pointB; })
      .add_property(
          "normal", +[](RobotWrapper::Distance& distance) { return distance.normal; })
      .add_property("min_distance", &RobotWrapper::Distance::min_distance);

  class_<RobotWrapper> robotWrapper =
      class__<RobotWrapper>("RobotWrapper", init<std::string, optional<int, std::string>>());
  exposeRobotType<RobotWrapper>(robotWrapper);

  class_<HumanoidRobot, bases<RobotWrapper>> humanoidWrapper =
      class__<HumanoidRobot, bases<RobotWrapper>>("HumanoidRobot", init<std::string, optional<int, std::string>>());

  exposeRobotType<HumanoidRobot>(humanoidWrapper);
  humanoidWrapper
      .def<void (HumanoidRobot::*)(const std::string&)>("update_support_side", &HumanoidRobot::update_support_side)
      .def("ensure_on_floor", &HumanoidRobot::ensure_on_floor)
      .def("get_T_world_left", &HumanoidRobot::get_T_world_left)
      .def("get_T_world_right", &HumanoidRobot::get_T_world_right)
      .def("get_T_world_trunk", &HumanoidRobot::get_T_world_trunk)
      .def("get_com_velocity", &HumanoidRobot::get_com_velocity)
      .def("dcm", &HumanoidRobot::dcm)
      .def("zmp", &HumanoidRobot::zmp)
      .def("other_side", &HumanoidRobot::other_side)
#ifdef HAVE_RHOBAN_UTILS
      .def("read_from_histories", &HumanoidRobot::read_from_histories, read_from_histories_overloads())
#endif
      .def(
          "get_support_side", +[](const HumanoidRobot& robot) { return robot.support_side; })
      .add_property("support_is_both", &HumanoidRobot::support_is_both, &HumanoidRobot::support_is_both)
      .add_property(
          "T_world_support", +[](HumanoidRobot& robot) { return robot.T_world_support; },
          +[](HumanoidRobot& robot, Eigen::Affine3d T_world_support_) { robot.T_world_support = T_world_support_; });

  exposeStdVector<RobotWrapper::Collision>("vector_Collision");
  exposeStdVector<RobotWrapper::Distance>("vector_Distance");
}
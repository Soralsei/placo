#include <pinocchio/fwd.hpp>

#include <Eigen/Dense>
#include <boost/python.hpp>
#include "placo/tools/axises_mask.h"
#include "module.h"
#include "registry.h"
#include "placo/tools/utils.h"
#include "expose-utils.hpp"
#ifdef HAVE_RHOBAN_UTILS
#include "rhoban_utils/history/history.h"
#endif

using namespace boost::python;
using namespace placo::tools;

#ifdef HAVE_RHOBAN_UTILS
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(loadReplays_overloads, loadReplays, 1, 2);
#endif

void exposeUtils()
{
  def("interpolate_frames", &interpolate_frames);
  def("wrap_angle", &wrap_angle);
  def("rotation_from_axis", &rotation_from_axis);
  def("frame_yaw", &frame_yaw);
  def("frame", &frame);
  def("flatten_on_floor", &flatten_on_floor);

  exposeStdVector<int>("vector_int");
  exposeStdVector<double>("vector_double");
  exposeStdVector<std::string>("vector_string");
  exposeStdVector<Eigen::MatrixXd>("vector_MatrixXd");

  // exposeStdMap<std::string, double>("map_string_double");

  class_<std::map<std::string, double>>("map_string_double").def(map_indexing_suite<std::map<std::string, double>>());

  class__<AxisesMask>("AxisesMask", init<>())
      .def<void (AxisesMask::*)(std::string, std::string)>("set_axises", &AxisesMask::set_axises)
      .add_property(
          "R_local_world", +[](AxisesMask& mask) { return mask.R_local_world; },
          +[](AxisesMask& mask, Eigen::Matrix3d R) { mask.R_local_world = R; })
      .add_property(
          "R_custom_world", +[](AxisesMask& mask) { return mask.R_custom_world; },
          +[](AxisesMask& mask, Eigen::Matrix3d R) { mask.R_custom_world = R; })
      .def("apply", &AxisesMask::apply);

#ifdef HAVE_RHOBAN_UTILS
  using namespace rhoban_utils;

  // History collection
  class__<HistoryCollection>("HistoryCollection")
      .def("loadReplays", &HistoryCollection::loadReplays, loadReplays_overloads())
      .def("smallestTimestamp", &HistoryCollection::smallestTimestamp)
      .def("biggestTimestamp", &HistoryCollection::biggestTimestamp)
      .def("startNamedLog", &HistoryCollection::startNamedLog)
      .def("stopNamedLog", &HistoryCollection::stopNamedLog)
      .def(
          "number", +[](HistoryCollection& collection, std::string name,
                        double t) { return collection.number(name)->interpolate(t); })
      .def(
          "angle", +[](HistoryCollection& collection, std::string name,
                       double t) { return collection.angle(name)->interpolate(t); })
      .def(
          "pose", +[](HistoryCollection& collection, std::string name,
                      double t) { return collection.pose(name)->interpolate(t); })
      .def(
          "bool", +[](HistoryCollection& collection, std::string name,
                      double t) { return collection.boolean(name)->interpolate(t); })
      .def(
          "push_number", +[](HistoryCollection& collection, std::string name, double t,
                             double value) { collection.number(name)->pushValue(t, value); })
      .def(
          "push_angle", +[](HistoryCollection& collection, std::string name, double t,
                            double value) { collection.angle(name)->pushValue(t, value); })
      .def(
          "push_pose", +[](HistoryCollection& collection, std::string name, double t,
                           Eigen::Affine3d value) { collection.pose(name)->pushValue(t, value); })
      .def(
          "push_bool", +[](HistoryCollection& collection, std::string name, double t,
                           bool value) { collection.boolean(name)->pushValue(t, value); })
      .def(
          "get_sequence",
          +[](HistoryCollection& collection, std::vector<std::string> entries, double start_t, double dt, int length) {
            Eigen::MatrixXd result(length, entries.size());

            for (int i = 0; i < length; i++)
            {
              double t = start_t + i * dt;
              for (int j = 0; j < entries.size(); j++)
              {
                result(i, j) = collection.number(entries[j])->interpolate(t);
              }
            }

            return result;
          })

      ;
#endif
}

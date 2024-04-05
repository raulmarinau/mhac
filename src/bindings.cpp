#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include "physics/SA.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(physics::Cities);

PYBIND11_MODULE(mhac, m) {
    py::module m_physics = m.def_submodule("physics");

    py::class_<physics::City>(m_physics, "City")
        .def(py::init<int, int>())
        .def_readwrite("x", &physics::City::x)
        .def_readwrite("y", &physics::City::y)
        .def("distance", &physics::City::distance);

    py::bind_vector<physics::Cities>(m_physics, "Cities");

    py::class_<physics::Problem, std::unique_ptr<physics::Problem>>(m_physics, "Problem");

    py::class_<physics::TSP, physics::Problem>(m_physics, "TSP")
        .def(py::init<const physics::Cities&>())
        .def_readwrite("mCities", &physics::TSP::mCities);

    py::class_<physics::SimulatedAnnealing>(m_physics, "SimulatedAnnealing")
        .def(py::init<physics::Problem*>())
        .def("solve", &physics::SimulatedAnnealing::solve)
        .def("getSolution", &physics::SimulatedAnnealing::getSolution);
}

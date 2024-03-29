#include <pybind11/pybind11.h>
#include "physics/SA.hpp"

namespace py = pybind11;

PYBIND11_MODULE(mhac, m) {
    py::class_<physics::SimulatedAnnealing>(m, "SimulatedAnnealing")
        .def(py::init<>())
        .def("solve", &physics::SimulatedAnnealing::solve);
}
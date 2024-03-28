#include <pybind11/pybind11.h>
#include "example_calculator.hpp"

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    py::class_<Calculator>(m, "Calculator")
        .def(py::init<>())
        .def("add", &Calculator::add);
}
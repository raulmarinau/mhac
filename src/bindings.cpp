#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include "physics/SA.hpp"
#include "problems/TSP.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(physics::SA::Cities);

PYBIND11_MODULE(mhac, m)
{
    py::module m_physics = m.def_submodule("physics");

    py::class_<physics::SA::SimulatedAnnealing>(m_physics, "SimulatedAnnealing")
        .def(py::init<physics::SA::ProblemPtr>())
        .def("solve", &physics::SA::SimulatedAnnealing::solve)
        .def("getSolution", &physics::SA::SimulatedAnnealing::getSolution);

    py::class_<physics::SA::Problem, physics::SA::PyProblem, physics::SA::ProblemPtr>(m_physics, "Problem")
        .def(py::init<>())
        .def("generateInitialSolution", &physics::SA::Problem::generateInitialSolution)
        .def("generateNewSolution", &physics::SA::Problem::generateNewSolution)
        .def("evaluateSolution", &physics::SA::Problem::evaluateSolution);

    py::class_<physics::SA::Solution, physics::SA::SolutionPtr>(m_physics, "Solution")
        .def(py::init<>());

    py::module m_physics_examples = m_physics.def_submodule("examples");

    py::class_<physics::SA::TSP, physics::SA::TSPPtr, physics::SA::Problem>(m_physics_examples, "TSP")
        .def(py::init<const physics::SA::Cities&>())
        .def_readwrite("mCities", &physics::SA::TSP::mCities);

    py::class_<physics::SA::TSS, physics::SA::TSSPtr>(m_physics_examples, "TSS")
        .def_readonly("tour", &physics::SA::TSS::tour);
    
    py::class_<physics::SA::City>(m_physics_examples, "City")
        .def(py::init<int, int>())
        .def_readwrite("x", &physics::SA::City::x)
        .def_readwrite("y", &physics::SA::City::y)
        .def("distance", &physics::SA::City::distance);
    py::bind_vector<physics::SA::Cities>(m_physics_examples, "Cities");
}

#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include "physics/SA.hpp"
#include "problems/TSP.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(problems::Cities);
PYBIND11_MAKE_OPAQUE(std::vector<int>);  // without this and the binding, int vector TSP::tour is immutable

PYBIND11_MODULE(mhac, m)
{
    // import mhac
    using VectorInt = std::vector<int>;
    py::bind_vector<VectorInt>(m, "VectorInt");
    py::implicitly_convertible<py::iterable, VectorInt>();

    // import mhac.common
    py::module m_common = m.def_submodule("common");

    py::class_<common::Problem, common::PyProblem, common::ProblemPtr>(m_common, "Problem")
        .def(py::init<>())
        .def("generateInitialSolution", &common::Problem::generateInitialSolution)
        .def("generateNewSolution", &common::Problem::generateNewSolution)
        .def("evaluateSolution", &common::Problem::evaluateSolution);

    py::class_<common::Solution, common::SolutionPtr>(m_common, "Solution")
        .def(py::init<>());
    
    // import mhac.physics
    py::module m_physics = m.def_submodule("physics");

    py::class_<physics::SA::SimulatedAnnealing>(m_physics, "SimulatedAnnealing")
        .def(py::init<common::ProblemPtr>())
        .def("solve", &physics::SA::SimulatedAnnealing::solve)
        .def("getSolution", &physics::SA::SimulatedAnnealing::getSolution);

    // import mhac.problems
    py::module m_problems = m.def_submodule("problems");
    
    py::class_<problems::City>(m_problems, "City")
        .def(py::init<int, int>())
        .def_readwrite("x", &problems::City::x)
        .def_readwrite("y", &problems::City::y)
        .def("distance", &problems::City::distance);
    py::bind_vector<problems::Cities>(m_problems, "Cities");

    py::class_<problems::TSP, common::Problem, problems::TSPPtr>(m_problems, "TSP")
        .def(py::init<const problems::Cities&>())
        .def_readwrite("mCities", &problems::TSP::mCities);

    py::class_<problems::TSS, common::Solution, problems::TSSPtr>(m_problems, "TSS")
        .def(py::init<>())
        .def_readwrite("tour", &problems::TSS::tour);
}

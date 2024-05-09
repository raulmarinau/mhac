#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include "common.hpp"
#include "math/TS.hpp"
#include "physics/SA.hpp"
#include "problems/TSP.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(problems::tsp::Cities);
PYBIND11_MAKE_OPAQUE(std::vector<int>);  // without this and the binding, int vector TSP::tour is immutable

PYBIND11_MODULE(mhac, m)
{
    using VectorInt = std::vector<int>;
    py::bind_vector<VectorInt>(m, "VectorInt");
    py::implicitly_convertible<py::iterable, VectorInt>();

    // import mhac
    // import mhac.common
    py::module m_common = m.def_submodule("common");

    py::class_<common::Problem, common::PyProblem, common::ProblemPtr>(m_common, "Problem")
        .def(py::init<>())
        .def("generateInitialSolution", &common::Problem::generateInitialSolution)
        .def("generateNewSolution", &common::Problem::generateNewSolution)
        .def("evaluateSolution", &common::Problem::evaluateSolution);

    py::class_<common::Solution, common::PySolution, common::SolutionPtr>(m_common, "Solution")
        .def(py::init<>());
    
    // import mhac.physics
    py::module m_physics = m.def_submodule("physics");

    py::class_<physics::SA::SimulatedAnnealing>(m_physics, "SimulatedAnnealing")
        .def(py::init<common::ProblemPtr>(), py::arg("problem"))
        .def("solve", &physics::SA::SimulatedAnnealing::solve, py::arg("maxT"), py::arg("minT"), py::arg("k"))
        .def("getSolution", &physics::SA::SimulatedAnnealing::getSolution);

    // import mhac.math
    py::module m_math = m.def_submodule("math");

    py::class_<math::TS::TabuSearch>(m_math, "TabuSearch")
        .def(py::init<common::ProblemPtr>(), py::arg("problem"))
        .def("solve", &math::TS::TabuSearch::solve, py::arg("iterations"), py::arg("maxTabuListSize"), py::arg("neighborhoodSize"))
        .def("getSolution", &math::TS::TabuSearch::getSolution);

    // import mhac.problems
    py::module m_problems = m.def_submodule("problems");
    py::module m_problems_tsp = m_problems.def_submodule("tsp");
    
    py::class_<problems::tsp::City>(m_problems_tsp, "City")
        .def(py::init<int, int>())
        .def_readwrite("x", &problems::tsp::City::x)
        .def_readwrite("y", &problems::tsp::City::y)
        .def("distance", &problems::tsp::City::distance);
    py::bind_vector<problems::tsp::Cities>(m_problems_tsp, "Cities");

    py::class_<problems::tsp::TSP, common::Problem, problems::tsp::TSPPtr>(m_problems_tsp, "TSP")
        .def(py::init<const problems::tsp::Cities&>(), py::arg("cities"))
        .def_readwrite("mCities", &problems::tsp::TSP::mCities);

    py::class_<problems::tsp::TSS, common::Solution, problems::tsp::TSSPtr>(m_problems_tsp, "TSS")
        .def(py::init<>())
        .def_readwrite("tour", &problems::tsp::TSS::tour);
}

#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include <pybind11/cast.h>

#include "common.hpp"
#include "logger/logger.hpp"

#include "physics/SA.hpp"
#include "math/TS.hpp"
#include "evolutionary/GA.hpp"
#include "swarm/ACO.hpp"

#include "problems/TSP.hpp"
#include "problems/JSS.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<int>);  // without this and the binding, int vector TSP::tour is immutable
PYBIND11_MAKE_OPAQUE(problems::tsp::Cities);
PYBIND11_MAKE_OPAQUE(problems::jss::TimeMatrix);

PYBIND11_MODULE(mhac, m)
{
    // import mhac
    using VectorInt = std::vector<int>;
    py::bind_vector<VectorInt>(m, "VectorInt");
    py::implicitly_convertible<py::iterable, VectorInt>();

    m.def("set_log_level", &setLogLevel, "Setup the log level");
    
    py::enum_<spdlog::level::level_enum>(m, "LogLevel")
        .value("trace", spdlog::level::trace)
        .value("debug", spdlog::level::debug)
        .value("info", spdlog::level::info)
        .value("warn", spdlog::level::warn)
        .value("err", spdlog::level::err)
        .value("critical", spdlog::level::critical)
        .value("off", spdlog::level::off)
        .export_values();

    // import mhac.common
    py::module m_common = m.def_submodule("common");

    py::class_<common::Problem, common::PyProblem, common::ProblemPtr>(m_common, "Problem")
        .def(py::init<>())
        .def("generateInitialSolution", &common::Problem::generateInitialSolution)
        .def("generateNewSolution", &common::Problem::generateNewSolution)
        .def("evaluateSolution", &common::Problem::evaluateSolution);

    py::class_<common::Solution, common::PySolution, common::SolutionPtr>(m_common, "Solution")
        .def(py::init<>())
        .def_readwrite("cost", &common::Solution::cost);
    
    // import mhac.physics
    py::module m_physics = m.def_submodule("physics");

    py::class_<physics::SA::SimulatedAnnealing>(m_physics, "SimulatedAnnealing")
        .def(py::init<common::ProblemPtr>(), py::arg("problem"))
        .def("solve", &physics::SA::SimulatedAnnealing::solve, py::arg("maxT"), py::arg("minT"), py::arg("k"));

    // import mhac.math
    py::module m_math = m.def_submodule("math");

    py::class_<math::TS::TabuSearch>(m_math, "TabuSearch")
        .def(py::init<common::ProblemPtr>(), py::arg("problem"))
        .def("solve", &math::TS::TabuSearch::solve, py::arg("iterations"), py::arg("maxTabuListSize"), py::arg("neighborhoodSize"));

    // import mhac.evolutionary
    py::module m_evolutionary = m.def_submodule("evolutionary");

    py::class_<evolutionary::GA::Problem, evolutionary::GA::PyProblem, evolutionary::GA::ProblemPtr>(m_evolutionary, "Problem")
        .def(py::init<>())
        .def("crossover", &evolutionary::GA::Problem::crossover)
        .def("mutation", &evolutionary::GA::Problem::mutation)
        .def("repair", &evolutionary::GA::Problem::repair);

    py::enum_<evolutionary::GA::SelectionType>(m_evolutionary, "SelectionType")
        .value("TOURNAMENT", evolutionary::GA::SelectionType::TOURNAMENT)
        .value("PROPORTIONAL", evolutionary::GA::SelectionType::PROPORTIONAL);

    py::class_<evolutionary::GA::GeneticAlgorithm>(m_evolutionary, "GeneticAlgorithm")
        .def(py::init<evolutionary::GA::ProblemPtr>(), py::arg("problem"))
        .def("solve", &evolutionary::GA::GeneticAlgorithm::solve, py::arg("generations"), py::arg("populationSize"), py::arg("mutationChance"), py::arg("selectionSize"), py::arg("selectionType"));

    // import mhac.swarm
    py::module m_swarm = m.def_submodule("swarm");

    py::class_<swarm::ACO::Problem, swarm::ACO::PyProblem, swarm::ACO::ProblemPtr>(m_swarm, "Problem")
        .def(py::init<>())
        .def("updateAntPath", &swarm::ACO::Problem::updateAntPath)
        .def("updatePheromoneMatrix", &swarm::ACO::Problem::updatePheromoneMatrix);

    py::class_<swarm::ACO::AntColonyOptimization>(m_swarm, "AntColonyOptimization")
        .def(py::init<swarm::ACO::ProblemPtr>(), py::arg("problem"))
        .def("solve", &swarm::ACO::AntColonyOptimization::solve, py::arg("generations"), py::arg("colonySize"), py::arg("alpha"), py::arg("beta"), py::arg("rho"));

    // import mhac.problems
    py::module m_problems = m.def_submodule("problems");

    // import mhac.problems.tsp
    py::module m_problems_tsp = m_problems.def_submodule("tsp");
    
    py::class_<problems::tsp::City>(m_problems_tsp, "City")
        .def(py::init<int, int>())
        .def_readwrite("x", &problems::tsp::City::x)
        .def_readwrite("y", &problems::tsp::City::y)
        .def("distance", &problems::tsp::City::distance);
    py::bind_vector<problems::tsp::Cities>(m_problems_tsp, "Cities");

    py::class_<problems::tsp::TSS, common::Solution, problems::tsp::TSSPtr>(m_problems_tsp, "TSS")
        .def(py::init<>())
        .def_readwrite("tour", &problems::tsp::TSS::tour)
        .def("print", &problems::tsp::TSS::print);

    py::class_<problems::tsp::TSP, common::Problem, problems::tsp::TSPPtr>(m_problems_tsp, "TSP")
        .def(py::init<const problems::tsp::Cities&>(), py::arg("cities"))
        .def_readwrite("cities", &problems::tsp::TSP::cities);

    py::class_<problems::tsp::GA_TSP, evolutionary::GA::Problem, problems::tsp::GA_TSPPtr>(m_problems_tsp, "GA_TSP")
        .def(py::init<const problems::tsp::Cities&>(), py::arg("cities"))
        .def_readwrite("cities", &problems::tsp::TSP::cities);

    py::class_<problems::tsp::ACO_TSP, swarm::ACO::Problem, problems::tsp::ACO_TSPPtr>(m_problems_tsp, "ACO_TSP")
        .def(py::init<const problems::tsp::Cities&>(), py::arg("cities"))
        .def_readwrite("cities", &problems::tsp::TSP::cities);

    // import mhac.problems.jss
    py::module m_problems_jss = m_problems.def_submodule("jss");

    py::bind_vector<problems::jss::TimeMatrix>(m_problems_jss, "TimeMatrix");

    py::class_<problems::jss::JSSS, common::Solution, problems::jss::JSSSPtr>(m_problems_jss, "JSSS")
        .def(py::init<>())
        .def_readwrite("schedule", &problems::jss::JSSS::schedule)
        .def("print", &problems::jss::JSSS::print);

    py::class_<problems::jss::JSSP, common::Problem, problems::jss::JSSPPtr>(m_problems_jss, "JSSP")
        .def(py::init<const problems::jss::TimeMatrix&>())
        .def_readwrite("products", &problems::jss::JSSP::products);

    py::class_<problems::jss::GA_JSSP, evolutionary::GA::Problem, problems::jss::GA_JSSPPtr>(m_problems_jss, "GA_JSSP")
        .def(py::init<const problems::jss::TimeMatrix&>())
        .def_readwrite("products", &problems::jss::JSSP::products);

    py::class_<problems::jss::ACO_JSSP, swarm::ACO::Problem, problems::jss::ACO_JSSPPtr>(m_problems_jss, "ACO_JSSP")
        .def(py::init<const problems::jss::TimeMatrix&>())
        .def_readwrite("products", &problems::jss::JSSP::products);
}

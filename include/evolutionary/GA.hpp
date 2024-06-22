#ifndef MHAC_EVOLUTIONARY_GA_HPP
#define MHAC_EVOLUTIONARY_GA_HPP

#include "common.hpp"
#include "pybind11/pybind11.h"

namespace evolutionary
{
namespace GA
{

class Problem : virtual public common::Problem
{
public:
    virtual common::SolutionVec crossover(common::SolutionPtr parent1, common::SolutionPtr parent2) = 0;
    virtual common::SolutionPtr mutation(common::SolutionPtr outChild, float mutationChance) = 0;
};
using ProblemPtr = std::shared_ptr<Problem>;

class PyProblem : public Problem, public common::PyProblem
{
public:
    using Problem::Problem;

    common::SolutionVec crossover(common::SolutionPtr parent1, common::SolutionPtr parent2) override
    {
        PYBIND11_OVERRIDE_PURE(common::SolutionVec, Problem, crossover, parent1, parent2);
    }

    common::SolutionPtr mutation(common::SolutionPtr outChild, float mutationChance) override
    {
        PYBIND11_OVERRIDE_PURE(common::SolutionPtr, Problem, mutation, outChild, mutationChance);
    }
};
using PyProblemPtr = std::shared_ptr<PyProblem>;


enum class SelectionType
{
    TOURNAMENT,
    PROPORTIONAL,
    NUM_SELECTIONS
};

class GeneticAlgorithm
{
public:
    GeneticAlgorithm() = delete;
    explicit GeneticAlgorithm(ProblemPtr);
    GeneticAlgorithm(const GeneticAlgorithm&) = delete;
    GeneticAlgorithm(GeneticAlgorithm&&) = delete;
    GeneticAlgorithm& operator=(const GeneticAlgorithm&) = delete;
    GeneticAlgorithm& operator=(GeneticAlgorithm&&) = delete;
    virtual ~GeneticAlgorithm() = default;
    
    common::SolutionPtr solve(int generations, int populationSize, float mutationChance, int selectionSize, SelectionType selectionType);

private:
    common::SolutionPtr tournamentSelection();
    common::SolutionPtr proportionalSelection();

    common::SolutionVec mPopulation;
    ProblemPtr mProblem;
    int mSelectionSize;
};

} // namespace GA
} // namespace evolutionary

#endif // MHAC_EVOLUTIONARY_GA_HPP

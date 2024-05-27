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
    virtual void crossover(common::SolutionPtr parent1, common::SolutionPtr parent2, common::SolutionPtr& outChild1, common::SolutionPtr& outChild2) = 0;
    virtual void mutation(common::SolutionPtr& outChild, float mutationChance) = 0;
    virtual void repair(common::SolutionPtr&) = 0;
};
using ProblemPtr = std::shared_ptr<Problem>;

class PyProblem : public Problem, public common::PyProblem
{
public:
    using Problem::Problem;

    void crossover(common::SolutionPtr parent1, common::SolutionPtr parent2, common::SolutionPtr& outChild1, common::SolutionPtr& outChild2) override
    {
        PYBIND11_OVERRIDE_PURE(void, Problem, crossover, parent1, parent2, outChild1, outChild2);
    }

    void mutation(common::SolutionPtr& outChild, float mutationChance) override
    {
        PYBIND11_OVERRIDE_PURE(void, Problem, mutation, outChild, mutationChance);
    }

    void repair(common::SolutionPtr& sol) override
    {
        PYBIND11_OVERRIDE_PURE(void, Problem, repair, sol);
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

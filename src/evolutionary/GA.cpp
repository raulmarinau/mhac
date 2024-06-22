#include <limits>
#include <algorithm>
#include <memory>

#include "common.hpp"
#include "logger/logger.hpp"
#include "random/random.hpp"

#include "evolutionary/GA.hpp"

namespace evolutionary
{
namespace GA
{

GeneticAlgorithm::GeneticAlgorithm(ProblemPtr probType)
    :mProblem(probType), mSelectionSize(0)
{
    globalLogger->flush_on(spdlog::level::err);
    globalLogger->debug("Initializing GeneticAlgorithm");
}

common::SolutionPtr GeneticAlgorithm::tournamentSelection()
{
    std::vector<int> indexes = mhac_random::sample(mPopulation.size(), mSelectionSize);

    int minElement = std::numeric_limits<int>::max();
    int indexOfMin = -1;

    for (const int index: indexes)
    {
        if (mPopulation[index]->cost < minElement)
        {
            minElement = mPopulation[index]->cost;
            indexOfMin = index;
        }
    }    
    return mPopulation[indexOfMin];
}

common::SolutionPtr GeneticAlgorithm::proportionalSelection()
{
    return nullptr;
}

common::SolutionPtr GeneticAlgorithm::solve(int generations, int populationSize, float mutationChance, int selectionSize, SelectionType selectionType)
{
    mSelectionSize = selectionSize;

    // initialization
    for (int i = 0; i < populationSize; i++)
    {
        auto sol = mProblem->generateInitialSolution();
        sol->cost = mProblem->evaluateSolution(sol);
        mPopulation.push_back(sol);
    }

    for (int gen = 0; gen < generations; gen++)
    {
        common::SolutionVec children;

        while ((int) children.size() < populationSize)
        {
            common::SolutionPtr parent1, parent2;

            // selection
            switch (selectionType)
            {
                case SelectionType::TOURNAMENT:
                {
                    parent1 = tournamentSelection();
                    parent2 = tournamentSelection();
                    break;
                }

                default:
                    break;
            }

            // crossover
            common::SolutionPtr child1, child2;
            common::SolutionVec res = mProblem->crossover(parent1, parent2);
            child1 = res[0];
            child2 = res[1];

            // mutation
            child1 = mProblem->mutation(child1, mutationChance);
            child2 = mProblem->mutation(child2, mutationChance);

            // create new generation
            children.push_back(child1);

            if ((int) children.size() < populationSize)
                children.push_back(child2);
        }

        mPopulation = children;
    }

    // return the best from the population
    return *std::min_element(mPopulation.begin(), mPopulation.end(), [](const common::SolutionPtr& a, const common::SolutionPtr& b) {
        return a->cost < b->cost;
    });
}

} // namespace GA
} // namespace evolutionary

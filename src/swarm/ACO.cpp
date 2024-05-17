#include "common.hpp"
#include "logger/logger.hpp"
#include "random/random.hpp"

#include "swarm/ACO.hpp"

namespace swarm
{
namespace ACO
{

PheromoneMatrix::PheromoneMatrix(int size, float initialValue)
    : size_(size), matrix_(size, std::vector<float>(size, initialValue))
{}

AntColonyOptimization::AntColonyOptimization(ProblemPtr probType)
    :mProblem(probType), mPheromoneMatrix(nullptr)
{
    globalLogger->flush_on(spdlog::level::err);
    globalLogger->debug("Initializing AntColonyOptimization");
}

common::SolutionPtr AntColonyOptimization::solve(int generations, int colonySize, float alpha, float beta, float rho)
{
    common::SolutionPtr bestS = mProblem->generateInitialSolution();
    bestS->cost = mProblem->evaluateSolution(bestS);

    mPheromoneMatrix = std::make_shared<PheromoneMatrix>(bestS->getSize(), 0);

    for (int i = 0; i < mPheromoneMatrix->getSize(); i++) {
        for (int j = 0; j < mPheromoneMatrix->getSize(); j++) {
            (*mPheromoneMatrix)(i, j) = mhac_random::random();
        }
    }

    for (int gen = 0; gen < generations; gen++)
    {
        for (int k = 0; k < colonySize; k++)
        {
            common::SolutionPtr ant = mProblem->generateInitialSolution();

            mProblem->updateAntPath(ant, mPheromoneMatrix, alpha, beta);
            ant->cost = mProblem->evaluateSolution(ant);

            mProblem->updatePheromoneMatrix(ant, mPheromoneMatrix, rho);

            if (ant->cost < bestS->cost)
            {
                bestS = ant;
                bestS->cost = ant->cost;
            }
        }
    }

    return bestS;
}


} // namespace ACO
} // namespace swarm

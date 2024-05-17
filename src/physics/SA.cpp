#include <exception>
#include <iostream>
#include <string>
#include <string>
#include <utility>
#include <unordered_set>

#include <spdlog/spdlog.h>

#include "logger/logger.hpp"
#include "random/random.hpp"

#include "physics/SA.hpp"

namespace physics
{
namespace SA
{

SimulatedAnnealing::SimulatedAnnealing(common::ProblemPtr probType)
    :mProblem(probType)
{
    globalLogger->flush_on(spdlog::level::err);
    globalLogger->debug("Initializing SimulatedAnnealing");
}

bool SimulatedAnnealing::accept(float currCost, float newCost, float T)
{
    bool accepted = false;
    if (newCost <= currCost)
    {
        globalLogger->debug("Accepted lower new cost {} on temp {}", newCost, T);
        accepted = true;
    }
    if (mhac_random::random() < std::exp(-(newCost - currCost)/T))
    {
        globalLogger->debug("Random accept new cost on temp {}", newCost, T);
        accepted = true;
    }

    return accepted;
}

float SimulatedAnnealing::updateTemp(float T)
{
    return mK*T;
}

common::SolutionPtr SimulatedAnnealing::solve(float maxT, float minT, float k)
{
    mK = k;
    common::SolutionPtr S = mProblem->generateInitialSolution();
    S->cost = mProblem->evaluateSolution(S);

    common::SolutionPtr bestS = S;
    bestS->cost = S->cost;

    float T = maxT;

    while (T > minT)
    {
        common::SolutionPtr primeS = mProblem->generateNewSolution(S);
        primeS->cost = mProblem->evaluateSolution(primeS);

        if (accept(S->cost, primeS->cost, T))
        {
            S = primeS;
            S->cost = primeS->cost;
        }

        if (S->cost < bestS->cost)
        {
            bestS = S;
            bestS->cost = S->cost;
            globalLogger->info("Found better solution with cost {}", bestS->cost);
        }

        T = updateTemp(T);
    }

    return bestS;
}

} // namespace SA
} // namespace physics

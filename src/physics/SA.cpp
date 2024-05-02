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

SimulatedAnnealing::SimulatedAnnealing(ProblemPtr probType)
    :mProblem(probType)
{
    globalLogger->set_level(spdlog::level::trace);
    globalLogger->flush_on(spdlog::level::info);
    globalLogger->debug("Initializing SA");
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

void SimulatedAnnealing::solve(float maxT, float minT, float k)
{
    mK = k;
    SolutionPtr S = mProblem->generateInitialSolution();
    float SCost = mProblem->evaluateSolution(S);

    SolutionPtr bestS = S;
    float bestSCost = SCost;

    float T = maxT;

    while (T > minT)
    {
        SolutionPtr primeS = mProblem->generateNewSolution(S);
        float primeSCost = mProblem->evaluateSolution(primeS);

        if (accept(SCost, primeSCost, T))
        {
            S = primeS;
            SCost = primeSCost;
        }

        if (SCost < bestSCost)
        {
            bestS = S;
            bestSCost = SCost;
            globalLogger->info("Found better solution with cost {}", bestSCost);
        }

        T = updateTemp(T);
    }

    mSolution = bestS;
}

SolutionPtr SimulatedAnnealing::getSolution()
{
    return mSolution;
}

} // namespace SA
} // namespace physics

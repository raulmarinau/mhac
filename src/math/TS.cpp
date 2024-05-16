#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <string>
#include <utility>
#include <unordered_set>

#include <spdlog/spdlog.h>

#include "common.hpp"
#include "logger/logger.hpp"
#include "random/random.hpp"

#include "math/TS.hpp"

namespace math
{
namespace TS
{

TabuSearch::TabuSearch(common::ProblemPtr probType)
    :mProblem(probType)
{
    globalLogger->set_level(spdlog::level::trace);
    globalLogger->flush_on(spdlog::level::debug);
    globalLogger->debug("Initializing TabuSearch");
}

bool TabuSearch::inTabuList(const common::SolutionPtr& newSol)
{
    return std::any_of(mTabuList.begin(), mTabuList.end(), [&newSol](const common::SolutionPtr& mTabuListMember){
        return newSol->isEqual(*mTabuListMember);
    });
}

common::SolutionPtr TabuSearch::solve(int iterations, int maxmTabuListSize, int neighborhoodSize)
{
    common::SolutionPtr S = mProblem->generateInitialSolution();
    S->cost = mProblem->evaluateSolution(S);

    common::SolutionPtr bestS = S;
    bestS->cost = S->cost;

    mTabuList.push_back(S);

    for (int iter = 0; iter < iterations; iter++)
    {
        common::SolutionPtr targetNeighbor = mProblem->generateNewSolution(S);
        targetNeighbor->cost = mProblem->evaluateSolution(targetNeighbor);

        for (int i = 0; i < neighborhoodSize; i++)
        {
            common::SolutionPtr newNeighbor = mProblem->generateNewSolution(S);
            newNeighbor->cost =  mProblem->evaluateSolution(newNeighbor);

            if (!inTabuList(newNeighbor) && newNeighbor->cost < targetNeighbor->cost)
            {
                targetNeighbor = newNeighbor;
                targetNeighbor->cost = newNeighbor->cost;
            }
        }

        if (targetNeighbor->cost < S->cost)
        {
            S = targetNeighbor;
            S->cost = targetNeighbor->cost;

            if ((int) mTabuList.size() > maxmTabuListSize)
            {
                mTabuList.erase(mTabuList.begin());
            }

            mTabuList.push_back(S);
        }

        if (S->cost < bestS->cost)
        {
            bestS = S;
            bestS->cost = S->cost;
            globalLogger->debug("Found better solution with cost: " + std::to_string(bestS->cost));
        }
    }

    return bestS;
}

} // namespace TS
} // namespace math

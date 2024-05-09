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
    globalLogger->flush_on(spdlog::level::info);
    globalLogger->debug("Initializing TabuSearch");
}

bool TabuSearch::inTabuList(const common::SolutionPtr& newSol)
{
    return std::any_of(mTabuList.begin(), mTabuList.end(), [&newSol](const common::SolutionPtr& mTabuListMember){
        return newSol->isEqual(*mTabuListMember);
    });
}

void TabuSearch::solve(int iterations, int maxmTabuListSize, int neighborhoodSize)
{
    common::SolutionPtr S = mProblem->generateInitialSolution();
    float SCost = mProblem->evaluateSolution(S);

    common::SolutionPtr bestS = S;
    float bestSCost = SCost;

    mTabuList.push_back(S);

    for (int iter = 0; iter < iterations; iter++)
    {
        common::SolutionPtr targetNeighbor = mProblem->generateNewSolution(S);
        float targetNeighborCost = mProblem->evaluateSolution(targetNeighbor);

        for (int i = 0; i < neighborhoodSize; i++)
        {
            common::SolutionPtr newNeighbor = mProblem->generateNewSolution(S);
            float newNeighborCost =  mProblem->evaluateSolution(newNeighbor);

            if (!inTabuList(newNeighbor) && newNeighborCost < targetNeighborCost)
            {
                targetNeighbor = newNeighbor;
                targetNeighborCost = newNeighborCost;
            }
        }

        if (targetNeighborCost < SCost)
        {
            S = targetNeighbor;
            SCost = targetNeighborCost;

            if ((int) mTabuList.size() > maxmTabuListSize)
            {
                mTabuList.erase(mTabuList.begin());
            }

            mTabuList.push_back(S);
        }

        if (SCost < bestSCost)
        {
            bestS = S;
            bestSCost = SCost;
        }
    }

    mSolution = bestS;
    mSolutionCost = bestSCost;
}

common::SolutionPtr TabuSearch::getSolution()
{
    return mSolution;
}

float TabuSearch::getSolutionCost()
{
    return mSolutionCost;
}

} // namespace TS
} // namespace math

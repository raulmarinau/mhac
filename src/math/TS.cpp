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
    return std::any_of(tabuList.begin(), tabuList.end(), [&newSol](const common::SolutionPtr& tabuListMember){
        return newSol->isEqual(*tabuListMember);
    });
}

void TabuSearch::solve(int iterations, int tabuListSize, int neighborhoodSize)
{
    common::SolutionPtr S = mProblem->generateInitialSolution();
    float SCost = mProblem->evaluateSolution(S);

    common::SolutionPtr bestS = S;
    float bestSCost = SCost;

    tabuList.push_back(S);

    for (int iter = 0; iter < iterations; iter++)
    {
        common::SolutionVec neighborhood;

        for (int i = 0; i < neighborhoodSize; i++)
        {
            auto neighbor = mProblem->generateNewSolution(S);
            float neighborCost = mProblem->evaluateSolution(neighbor);

            if (!inTabuList(neighbor) && neighborCost < SCost)
            {
                neighborhood.push_back((neighbor));
            }
        }

        common::SolutionPtr bestNeighbor;
        float bestNeighborCost = mProblem->evaluateSolution(*neighborhood.begin());

        for (const common::SolutionPtr& sol : neighborhood)
        {
            float cost = mProblem->evaluateSolution(sol);
            if (cost < bestNeighborCost)
            {
                bestNeighbor = sol;
                bestNeighborCost = cost;
            } 
        }

        S = bestNeighbor;
        SCost = bestNeighborCost;

        if (SCost < bestSCost)
        {
            bestS = S;
            bestSCost = SCost;
        }

        // this ensures that the search does not
        // cycle back to recent solutions
        tabuList.push_back(S);

        if ((int) tabuList.size() > tabuListSize)
        {
            tabuList.erase(tabuList.begin());
        }
        mSolution = bestS;

        // breaks the loop if the cost of the best neighbor found
        // in the current iteration equals the current best cost
        // found over all iterations
        if (bestSCost == bestNeighborCost)
            break;
    }

}

common::SolutionPtr TabuSearch::getSolution()
{
    return mSolution;
}

} // namespace TS
} // namespace math

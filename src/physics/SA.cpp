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

TSP::TSP(const Cities& cities)
{
    mCities = cities;
}

float TSP::evaluateSolution(const Solution& sol)
{
    float ev = 0;
    int lastCityIndex = mCities.size() - 1;
    for (int i = 0; i < lastCityIndex; i++)
    {
        ev += mCities[sol[i]].distance(mCities[sol[i+1]]);
    }
    ev += mCities[sol[lastCityIndex]].distance(mCities[sol[0]]);
    return ev;
}

Solution TSP::generateInitialSolution()
{
    Solution sol = mhac_random::sample(mCities.size(), mCities.size());
    std::unordered_set<int> seen;

    // bool duplicate = false;
    // for (const int item : sol) {
    //     if (seen.find(item) != seen.end()) {
    //         duplicate = true;
    //     }
    //     seen.insert(item);
    // }
    // if (duplicate)
    //     globalLogger->warn("Duplicate found in initial solution");

    // std::string solStr;
    // for (int i = 0; i < (int) sol.size(); i++)
    // {
    //     solStr += std::to_string(sol[i]) + " ";
    // }
    // globalLogger->debug("Generated initial solution {}", solStr);

    return sol;
}

Solution TSP::generateNewSolution(const Solution& initialSol)
{
    Solution sol = initialSol;

    std::vector<int> indexes = mhac_random::sample(mCities.size(), 2);
    int i = indexes[0];
    int j = indexes[1];

    if (i > j)
        std::swap(i, j);

    for (int k = 0; k < (j-i+1) / 2; k++)
        std::swap(sol[i+k], sol[j-k]);

    // std::string solStr;
    // for (int i = 0; i < (int) sol.size(); i++)
    // {
    //     solStr += std::to_string(i) + " ";
    // }
    // globalLogger->debug("Generated new solution {}", solStr);

    return sol;
}

SimulatedAnnealing::SimulatedAnnealing(Problem* probType)
    : mProblemType(probType)
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
    return 0.9995*T;
}

void SimulatedAnnealing::solve(float maxT, float minT)
{
    Solution S = mProblemType->generateInitialSolution();
    float SCost = mProblemType->evaluateSolution(S);

    Solution bestS = S;
    float bestSCost = SCost;

    float T = maxT;

    while (T > minT)
    {
        Solution primeS = mProblemType->generateNewSolution(S);
        float primeSCost = mProblemType->evaluateSolution(primeS);

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

    mSol = bestS;
}

Solution SimulatedAnnealing::getSolution()
{
    return mSol;
}

} // namespace physics
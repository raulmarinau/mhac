#include <iostream>
#include <random>
#include <string>
#include <string>
#include <utility>

#include "logger/logger.hpp"
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
    ev += mCities[sol[0]].distance(mCities[sol[lastCityIndex]]);
    return ev;
}

Solution TSP::generateInitialSolution()
{
    Solution sol;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, mCities.size()-1);

    std::string solStr;
    for (int i = 0; i < (int) mCities.size(); i++)
    {
        int cityNr = distr(gen);
        solStr += std::to_string(cityNr) + " ";
        sol.push_back(cityNr);
    }

    globalLogger->debug("Generated initial solution {}", solStr);

    return sol;
}

Solution TSP::generateNewSolution(const Solution& initialSol)
{
    Solution sol = initialSol;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, mCities.size()-1);

    int i = distr(gen);
    int j = distr(gen);

    if (i > j)
        std::swap(i, j);

    for (int k = 0; k < (j-i) / 2; k++)
        std::swap(sol[i+k], sol[j-k]);

    std::string solStr;
    for (int i = 0; i < (int) sol.size(); i++)
    {
        solStr += std::to_string(i) + " ";
    }
    
    globalLogger->debug("Generated new solution {}", solStr);

    return sol;
}

SimulatedAnnealing::SimulatedAnnealing(Problem* probType)
    : mProblemType(probType)
{
    globalLogger->set_level(spdlog::level::trace);
    globalLogger->flush_on(spdlog::level::info);
    globalLogger->info("Initializing SA");
}

bool SimulatedAnnealing::accept(float currCost, float newCost, float T)
{
    globalLogger->info("Running accept");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 1);

    if (newCost <= currCost)
        return true;
    if (distr(gen) < std::exp(-(newCost - currCost)/T))
        return true;
    return false;
}

float SimulatedAnnealing::updateTemp(float T)
{
    globalLogger->info("Updating temp");
    return 0.9995*T;
}

void SimulatedAnnealing::solve(float maxT, float minT)
{
    globalLogger->info("Solve start");
    Solution currSol = mProblemType->generateInitialSolution();
    float SCost = mProblemType->evaluateSolution(currSol);

    Solution bestSol = currSol;
    float bestCost = SCost;

    float T = maxT;
    int k = 0;

    while (T > minT)
    {
        globalLogger->info("Current temp {}", T);
        k++;
        Solution primeS = mProblemType->generateNewSolution(mSol);
        float primeSCost = mProblemType->evaluateSolution(primeS);

        if (accept(SCost, primeSCost, T))
        {
            currSol = primeS;
            SCost = primeSCost;
            globalLogger->info("Accepted new sol with cost {}", SCost);
        }

        if (SCost < bestCost)
        {
            bestSol = currSol;
            bestCost = SCost;
            globalLogger->info("Found better sol {}", bestCost);
        }

        T = updateTemp(T);
    }

    mSol = bestSol;
}

Solution SimulatedAnnealing::getSolution()
{
    return mSol;
}

} // namespace physics
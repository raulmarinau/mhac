#include <iostream>
#include <random>
#include <utility>

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

    for (int i = 0; i < (int) mCities.size(); i++)
    {
        sol.push_back(distr(gen));
    }

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

    return sol;
}

SimulatedAnnealing::SimulatedAnnealing(Problem* probType)
    : mProblemType(probType)
{
}

bool SimulatedAnnealing::accept(float currCost, float newCost, float T)
{
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
    return 0.9995*T;
}

void SimulatedAnnealing::solve(float maxT, float minT)
{
    Solution currSol = mProblemType->generateInitialSolution();
    float SCost = mProblemType->evaluateSolution(currSol);

    Solution bestSol = currSol;
    float bestCost = SCost;

    float T = maxT;
    int k = 0;

    while (T > minT)
    {
        k++;
        Solution primeS = mProblemType->generateNewSolution(mSol);
        float primeSCost = mProblemType->evaluateSolution(primeS);

        if (accept(SCost, primeSCost, T))
        {
            currSol = primeS;
            SCost = primeSCost;
        }

        if (SCost < bestCost)
        {
            bestSol = currSol;
            bestCost = SCost;
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
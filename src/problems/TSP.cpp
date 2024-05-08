#include "problems/TSP.hpp"
#include "random/random.hpp"
#include <memory>

namespace problems
{

TSP::TSP(const Cities& cities)
{
    mCities = cities;
}

float TSP::evaluateSolution(common::SolutionPtr sol)
{
    TSSPtr tss = std::dynamic_pointer_cast<TSS>(sol);
    float ev = 0;
    int lastCityIndex = mCities.size() - 1;
    for (int i = 0; i < lastCityIndex; i++)
    {
        ev += mCities[tss->tour[i]].distance(mCities[tss->tour[i+1]]);
    }
    ev += mCities[tss->tour[lastCityIndex]].distance(mCities[tss->tour[0]]);
    return ev;
}

common::SolutionPtr TSP::generateInitialSolution()
{
    TSSPtr tss = std::make_shared<TSS>();
    tss->tour = mhac_random::sample(mCities.size(), mCities.size());
    return tss;
}

common::SolutionPtr TSP::generateNewSolution(common::SolutionPtr initialSol)
{
    TSSPtr tssInitial = std::dynamic_pointer_cast<TSS>(initialSol);
    TSSPtr tssNew = std::make_shared<TSS>();
    tssNew->tour = tssInitial->tour;

    std::vector<int> indexes = mhac_random::sample(mCities.size(), 2);
    int i = indexes[0];
    int j = indexes[1];

    if (i > j)
        std::swap(i, j);

    for (int k = 0; k < (j-i+1) / 2; k++)
        std::swap(tssNew->tour[i+k], tssNew->tour[j-k]);

    return tssNew;
}

} // namespace problems

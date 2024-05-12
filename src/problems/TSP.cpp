#include <memory>
#include <string>

#include "common.hpp"
#include "random/random.hpp"
#include "logger/logger.hpp"

#include "problems/TSP.hpp"

namespace problems
{
namespace tsp
{

bool TSS::isEqual(const Solution& other) const
{
    const TSS* otherTSS = dynamic_cast<const TSS*>(&other);
    return otherTSS && this->tour == otherTSS->tour;
}

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

GA_TSP::GA_TSP(const Cities& cities): TSP(cities)
{
    mCities = cities;
}

void GA_TSP::crossover(common::SolutionPtr parent1, common::SolutionPtr parent2, common::SolutionPtr& outChild1, common::SolutionPtr& outChild2)
{
    TSSPtr tss_parent1 = std::dynamic_pointer_cast<TSS>(parent1);
    TSSPtr tss_parent2 = std::dynamic_pointer_cast<TSS>(parent2);

    outChild1 = std::make_shared<TSS>();
    outChild2 = std::make_shared<TSS>();

    TSSPtr tss_outChild1 = std::dynamic_pointer_cast<TSS>(outChild1);
    TSSPtr tss_outChild2 = std::dynamic_pointer_cast<TSS>(outChild2);

    int cutPoint = mhac_random::random(0, tss_parent1->tour.size() - 1);

    tss_outChild1->tour.resize(tss_parent1->tour.size());
    tss_outChild2->tour.resize(tss_parent2->tour.size());

    // [0, cutPoint)
    std::copy(tss_parent1->tour.begin(), tss_parent1->tour.begin() + cutPoint, tss_outChild1->tour.begin());
    std::copy(tss_parent2->tour.begin(), tss_parent2->tour.begin() + cutPoint, tss_outChild2->tour.begin());

    // [cutPoint, v.size())
    std::copy(tss_parent2->tour.begin() + cutPoint, tss_parent2->tour.end(), tss_outChild1->tour.begin() + cutPoint);
    std::copy(tss_parent1->tour.begin() + cutPoint, tss_parent1->tour.end(), tss_outChild2->tour.begin() + cutPoint);
}

void GA_TSP::mutation(common::SolutionPtr outChild, float mutationChance)
{
    TSSPtr tss = std::dynamic_pointer_cast<TSS>(outChild);
    std::vector<int> indexes = mhac_random::sample(mCities.size(), 2);
    int i = indexes[0];
    int j = indexes[1];

    if (mhac_random::random() < mutationChance)
        std::swap(tss->tour[i], tss->tour[j]);
}

} // namespace tsp
} // namespace problems

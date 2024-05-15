#include <cmath>
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

int TSS::getSize() const
{
    return this->tour.size();
}

std::string TSS::print()
{
    std::string s;
    for (int i = 0; i < (int) tour.size(); i++)
    {
        s += std::to_string(tour[i]) + " ";
    }
    return s;
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

void GA_TSP::repair(common::SolutionPtr& sol)
{
    TSSPtr tss = std::dynamic_pointer_cast<TSS>(sol);
    std::unordered_set<int> seen;
    std::vector<int> missing;

    // identify all duplicates and records their positions
    for (int i = 0; i < (int) tss->tour.size(); i++)
    {
        if (seen.count(tss->tour[i])) {
            missing.push_back(i);
        }
        else {
            seen.insert(tss->tour[i]);
        }
    }

    // place them in the positions of duplicates
    for (int city = 0; city < (int) mCities.size(); city++)
    {
        if (!seen.count(city)) {
            tss->tour[missing.back()] = city;
            missing.pop_back();
        }
    }
}

void GA_TSP::crossover(common::SolutionPtr parent1, common::SolutionPtr parent2, common::SolutionPtr& outChild1, common::SolutionPtr& outChild2)
{
    TSSPtr tss_parent1 = std::dynamic_pointer_cast<TSS>(parent1);
    TSSPtr tss_parent2 = std::dynamic_pointer_cast<TSS>(parent2);

    globalLogger->debug("Parent1 (cost:" + std::to_string(tss_parent1->cost) + ")" + tss_parent1->print());
    globalLogger->debug("Parent2 (cost:" + std::to_string(tss_parent2->cost) + ")" + tss_parent2->print());

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

    // repair
    repair(outChild1);
    repair(outChild2);
    
    tss_outChild1->cost = evaluateSolution(tss_outChild1);
    tss_outChild2->cost = evaluateSolution(tss_outChild2);

    globalLogger->debug("Child1 (cost:" + std::to_string(tss_outChild1->cost) + ")" + tss_outChild1->print());
    globalLogger->debug("Child2 (cost:" + std::to_string(tss_outChild2->cost) + ")" + tss_outChild2->print());
}

void GA_TSP::mutation(common::SolutionPtr& outChild, float mutationChance)
{
    TSSPtr tss = std::dynamic_pointer_cast<TSS>(outChild);
    std::vector<int> indexes = mhac_random::sample(mCities.size(), 2);
    int i = indexes[0];
    int j = indexes[1];

    if (mhac_random::random() < mutationChance)
    {
        std::swap(tss->tour[i], tss->tour[j]);
        tss->cost = evaluateSolution(tss);

        globalLogger->debug("Mutation (cost:" + std::to_string(tss->cost) + ")" + tss->print());
    }
}

ACO_TSP::ACO_TSP(const Cities& cities): TSP(cities)
{
    mCities = cities;
}

void ACO_TSP::updateAntPath(common::SolutionPtr &ant, swarm::ACO::PheromoneMatrixPtr pm, float alpha, float beta)
{
    TSSPtr tss_ant = std::dynamic_pointer_cast<TSS>(ant);
    // globalLogger->debug("Starting tour: " + tss_ant->print());

    tss_ant->tour[0] = 0;

    std::vector<int> availableCitiesIndexes;
    for (int i = 1; i < (int) mCities.size(); i++) {
        availableCitiesIndexes.push_back(i);
    }

    // remove starting city in tour
    // availableCitiesIndexes.erase(availableCitiesIndexes.begin() + tss_ant->tour[0]);

    for (int node = 1; node < (int) tss_ant->getSize(); node++) {
        std::vector<float> probabilities(tss_ant->getSize(), 0);

        float sum = 0;

        for (const int cindex: availableCitiesIndexes) {
            float distance = mCities[tss_ant->tour[node-1]].distance(mCities[cindex]);
            if (distance == 0) {
                continue; // Optionally handle this case more gracefully
            }
            float eta = 1 / distance;
            float t = std::pow((*pm)(tss_ant->tour[node-1], cindex), alpha)  * std::pow(eta, beta);
            sum += t;
            probabilities[cindex] = t;
        }

        for (const int cindex: availableCitiesIndexes) {
            probabilities[cindex] /= sum;
        }

        // select the node
        int probIndex = 0;
        float s = probabilities[0];
        float u = mhac_random::random();

        while (u > s) {
            probIndex++;
            s += probabilities[probIndex]; 
        }

        // globalLogger->debug("Next city in tour: " + std::to_string(probIndex));

        availableCitiesIndexes.erase(std::find(availableCitiesIndexes.begin(), availableCitiesIndexes.end(), probIndex));

        tss_ant->tour[node] = probIndex;
    }

    // globalLogger->debug("Computed tour: " + tss_ant->print());
}

void ACO_TSP::updatePheromoneMatrix(common::SolutionPtr ant, swarm::ACO::PheromoneMatrixPtr &pm, float rho)
{
    TSSPtr tss_ant = std::dynamic_pointer_cast<TSS>(ant);
    for (int i = 0; i < pm->getSize(); i++) {
        for (int j = 0; j < pm->getSize(); j++) {
            float pheromoneDeposit = 0;

            for (int k = 1; k < pm->getSize()-1; k++) {
                if (tss_ant->tour[k] == i && tss_ant->tour[k+1] == j) {
                    pheromoneDeposit = 1 / (mCities[i].distance(mCities[j]));
                }
            }

            (*pm)(i, j) = (1-rho) * (*pm)(i, j) + rho*pheromoneDeposit;
        }
    }
}

} // namespace tsp
} // namespace problems

#include <queue>
#include <memory>
#include <string>

#include "logger/logger.hpp"
#include "random/random.hpp"

#include "problems/JSS.hpp"

namespace problems
{
namespace jss
{

bool JSSS::isEqual(const common::Solution & other) const
{
    const JSSS* otherJSSS = dynamic_cast<const JSSS*>(&other);
    return this->schedule == otherJSSS->schedule;
}

int JSSS::getSize() const
{
    return this->schedule.size();
}

std::string JSSS::print()
{
    std::string s;
    for (int i = 0; i < (int) schedule.size(); i++)
    {
        s += std::to_string(schedule[i]) + " ";
    }
    return s;
}

JSSP::JSSP(const TimeMatrix& products)
{
    this->products = products;
}

float JSSP::evaluateSolution(common::SolutionPtr sol)
{
    JSSSPtr sol_sequence = std::dynamic_pointer_cast<JSSS>(sol);
    
    int num_products = products.size();
    int num_machines = products[0].size();
    TimeMatrix completion_times(num_products, ProductVec(num_machines, 0));
    int total_completition_time = 0;

    for (int productIndex = 0; productIndex < num_products; productIndex++) {
        int product = sol_sequence->schedule[productIndex];

        for (int machine = 0; machine < num_machines; machine++) {
            if (productIndex == 0) {
                completion_times[productIndex][machine] = this->products[product][machine];
            }
            else {
                if (machine == 0) {
                    completion_times[productIndex][machine] =
                        completion_times[productIndex-1][machine] + this->products[product][machine];
                }
                else {
                    completion_times[productIndex][machine] = this->products[product][machine] +
                        std::max(completion_times[productIndex][machine-1], completion_times[productIndex-1][machine]);
                }
            }

            if (machine == num_machines - 1) {
                total_completition_time += completion_times[productIndex][machine];
            }
        }
    }

    globalLogger->debug("Schedule: " + sol_sequence->print());

    globalLogger->debug("Completition times:");

    for (int i = 0; i < num_products; i++) {
        std::string row;
        for (int j = 0; j < num_machines; j++) {
            row += std::to_string(completion_times[i][j]) + " ";
        }
        globalLogger->debug(row);
    }

    globalLogger->debug("Total cost: " + std::to_string(total_completition_time));

    return total_completition_time;
}

common::SolutionPtr JSSP::generateInitialSolution()
{
    JSSSPtr tss = std::make_shared<JSSS>();
    tss->schedule = mhac_random::sample(products.size(), products.size());
    return tss;
}

common::SolutionPtr JSSP::generateNewSolution(common::SolutionPtr initialSol)
{
    JSSSPtr jssInitial = std::dynamic_pointer_cast<JSSS>(initialSol);
    JSSSPtr jssNew = std::make_shared<JSSS>();
    jssNew->schedule = jssInitial->schedule;

    std::vector<int> indexes = mhac_random::sample(jssInitial->getSize(), 2);
    int i = indexes[0];
    int j = indexes[1];
    
    std::swap(jssNew->schedule[i], jssNew->schedule[j]);

    return jssNew;
}

GA_JSSP::GA_JSSP(const TimeMatrix& products) : JSSP(products)
{}

void GA_JSSP::repair(common::SolutionPtr& sol)
{
    JSSSPtr jss = std::dynamic_pointer_cast<JSSS>(sol);

    int size = jss->getSize();
    std::vector<int>& schedule = jss->schedule;

    std::unordered_set<int> seen;
    std::vector<int> duplicates;
    std::vector<bool> isPresent(size, false);

    // Identify duplicates and check which elements are present
    for (int i = 0; i < size; ++i) {
        if (seen.find(schedule[i]) != seen.end()) {
            // This machine is a duplicate
            duplicates.push_back(i);
        } else {
            seen.insert(schedule[i]);
            isPresent[schedule[i]] = true;
        }
    }

    // Find missing elements
    std::queue<int> missing;
    for (int i = 0; i < size; ++i) {
        if (!isPresent[i]) {
            missing.push(i);
        }
    }

    // Replace duplicates with missing elements
    for (int idx : duplicates) {
        if (!missing.empty()) {
            schedule[idx] = missing.front();
            missing.pop();
        }
    }
}

void GA_JSSP::crossover(common::SolutionPtr parent1, common::SolutionPtr parent2, common::SolutionPtr& outChild1, common::SolutionPtr& outChild2)
{
    JSSSPtr jss_parent1 = std::dynamic_pointer_cast<JSSS>(parent1);
    JSSSPtr jss_parent2 = std::dynamic_pointer_cast<JSSS>(parent2);

    outChild1 = std::make_shared<JSSS>();
    outChild2 = std::make_shared<JSSS>();

    JSSSPtr jss_outChild1 = std::dynamic_pointer_cast<JSSS>(outChild1);
    JSSSPtr jss_outChild2 = std::dynamic_pointer_cast<JSSS>(outChild2);

    int cutPoint = mhac_random::random(0, jss_parent1->schedule.size() - 1);

    jss_outChild1->schedule.resize(jss_parent1->schedule.size());
    jss_outChild2->schedule.resize(jss_parent2->schedule.size());

    // [0, cutPoint)
    std::copy(jss_parent1->schedule.begin(), jss_parent1->schedule.begin() + cutPoint, jss_outChild1->schedule.begin());
    std::copy(jss_parent2->schedule.begin(), jss_parent2->schedule.begin() + cutPoint, jss_outChild2->schedule.begin());

    // [cutPoint, v.size())
    std::copy(jss_parent2->schedule.begin() + cutPoint, jss_parent2->schedule.end(), jss_outChild1->schedule.begin() + cutPoint);
    std::copy(jss_parent1->schedule.begin() + cutPoint, jss_parent1->schedule.end(), jss_outChild2->schedule.begin() + cutPoint);

    // repair
    repair(outChild1);
    repair(outChild2);
    
    jss_outChild1->cost = evaluateSolution(jss_outChild1);
    jss_outChild2->cost = evaluateSolution(jss_outChild2);
}

void GA_JSSP::mutation(common::SolutionPtr& outChild, float mutationChance)
{
    JSSSPtr tss = std::dynamic_pointer_cast<JSSS>(outChild);
    std::vector<int> indexes = mhac_random::sample(outChild->getSize(), 2);
    int i = indexes[0];
    int j = indexes[1];

    if (mhac_random::random() < mutationChance)
    {
        std::swap(tss->schedule[i], tss->schedule[j]);
        tss->cost = evaluateSolution(tss);
    }
}

ACO_JSSP::ACO_JSSP(const TimeMatrix& products) : JSSP(products)
{}

void ACO_JSSP::updateAntPath(common::SolutionPtr &ant, swarm::ACO::PheromoneMatrixPtr pm, float alpha, float beta)
{
    JSSSPtr jss = std::dynamic_pointer_cast<JSSS>(ant);

    int N = jss->getSize();
    std::vector<float> probabilities(N, 0.0f);

    // TODO: consider using globals or moving to mhac_random
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist;

    for (int i = 0; i < N - 1; ++i) {
        int currentJob = jss->schedule[i];
        float sum = 0.0f;

        for (int j = i + 1; j < N; ++j) {
            int nextJob = jss->schedule[j];
            float pheromone = std::pow((*pm)(currentJob, nextJob), alpha);
            float heuristic = (this->products[currentJob][nextJob] == 0) ? 0 : (1.0 / this->products[currentJob][nextJob]);
            float eta = std::pow(heuristic, beta);
            probabilities[j] = pheromone * eta;
            sum += probabilities[j];
        }

        for (int j = i + 1; j < N; ++j) {
            probabilities[j] /= sum;
        }

        // Sample one index based on probabilities

        // random
        // std::vector<int> indices = mhac_random::sample(N - i - 1, 1); // Get one index from remaining
        // int selectedIndex = indices[0] + i + 1; // Adjust index to be relative to i + 1

        // weighted
        auto dist = std::discrete_distribution<>(probabilities.begin() + i + 1, probabilities.end());
        int selectedIndex = i + 1 + dist(gen);

        std::swap(jss->schedule[i + 1], jss->schedule[selectedIndex]);
    }
}

void ACO_JSSP::updatePheromoneMatrix(common::SolutionPtr ant, swarm::ACO::PheromoneMatrixPtr &pm, float rho)
{
    JSSSPtr jss = std::dynamic_pointer_cast<JSSS>(ant);

    float totalJobCompletionTime = evaluateSolution(ant);
    float deposit = 1.0 / (totalJobCompletionTime);

    for (int i = 0; i < pm->getSize(); i++) {
        for (int j = 0; j < pm->getSize(); j++) {
            (*pm)(i, j) *= (1 - rho); // Evaporate the existing pheromone
        }
    }

    // Deposit new pheromones based on the ant's path
    for (size_t k = 0; k < jss->schedule.size() - 1; ++k) {
        int i = jss->schedule[k];
        int j = jss->schedule[k + 1];
        (*pm)(i, j) += deposit; // Add new pheromones based on solution quality
    }
}

} // namespace jss
} // namespace problems

#ifndef MHAC_PROBLEMS_JSS_HPP
#define MHAC_PROBLEMS_JSS_HPP

#include <memory>
#include <vector>
#include <cmath>

#include "common.hpp"
#include "evolutionary/GA.hpp"
#include "swarm/ACO.hpp"

namespace problems
{
namespace jss
{

using ProductVec = std::vector<int>;
using TimeMatrix = std::vector<ProductVec>; // size = N (products) * M (machines)

// Job-Shop Scheduling Solution
class JSSS: public common::Solution
{
public:
    bool isEqual(const common::Solution&) const override;
    int getSize() const override;
    std::vector<int> schedule;
    std::string print();
};
using JSSSPtr = std::shared_ptr<JSSS>;

// Job-Shop Scheduling Problem
class JSSP: virtual public common::Problem
{
public:
    JSSP() = delete;
    explicit JSSP(const TimeMatrix&);
    
    common::SolutionPtr generateInitialSolution() override;
    common::SolutionPtr generateNewSolution(common::SolutionPtr) override;
    float evaluateSolution(common::SolutionPtr) override;
    
    TimeMatrix products;
};
using JSSPPtr = std::shared_ptr<JSSP>;

// Genetic Algorithm JSSP
class GA_JSSP : public evolutionary::GA::Problem, public JSSP
{
public:
    GA_JSSP() = delete;
    explicit GA_JSSP(const TimeMatrix&);

    common::SolutionVec crossover(common::SolutionPtr parent1, common::SolutionPtr parent2) override;
    common::SolutionPtr mutation(common::SolutionPtr outChild, float mutationChance) override;
    void repair(common::SolutionPtr);
};
using GA_JSSPPtr = std::shared_ptr<GA_JSSP>;

// Ant Colony Optimization JSSP
class ACO_JSSP : public swarm::ACO::Problem, public JSSP
{
public:
    ACO_JSSP() = delete;
    explicit ACO_JSSP(const TimeMatrix&);

    common::SolutionPtr updateAntPath(common::SolutionPtr ant, swarm::ACO::PheromoneMatrixPtr pm, float alpha, float beta) override;
    void updatePheromoneMatrix(common::SolutionPtr ant, swarm::ACO::PheromoneMatrixPtr pm, float rho) override;
};
using ACO_JSSPPtr = std::shared_ptr<ACO_JSSP>;

} // namespace jss
} // namespace problems

#endif // MHAC_PROBLEMS_JSS_HPP

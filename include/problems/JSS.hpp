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

} // namespace jss
} // namespace problems

#endif // MHAC_PROBLEMS_JSS_HPP

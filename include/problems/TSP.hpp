#ifndef MHAC_PROBLEMS_TSP_HPP
#define MHAC_PROBLEMS_TSP_HPP

#include <memory>
#include <vector>
#include <cmath>

#include "physics/SA.hpp"

namespace problems
{
namespace tsp
{

struct City
{
    int x;
    int y;

    inline float distance(const City& c) const
    {
        int xdis = this->x - c.x;
        int ydis = this->y - c.y;
        return sqrt(xdis*xdis + ydis*ydis);
    }
};
using Cities = std::vector<City>;

// Traveling Salesman Solution
class TSS: public common::Solution
{
public:
    bool isEqual(const Solution&) const override;
    std::vector<int> tour;
};
using TSSPtr = std::shared_ptr<TSS>;

// Traveling Salesman Problem
class TSP: public common::Problem
{
public:
    TSP() = delete;
    explicit TSP(const Cities&);
    
    common::SolutionPtr generateInitialSolution() override;
    common::SolutionPtr generateNewSolution(common::SolutionPtr) override;
    float evaluateSolution(common::SolutionPtr) override;
    
    Cities mCities;
};
using TSPPtr = std::shared_ptr<TSP>;

} // namespace tsp
} // namespace problems

#endif // MHAC_PROBLEMS_TSP_HPP

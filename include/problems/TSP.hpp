#ifndef MHAC_PROBLEMS_TSP_HPP
#define MHAC_PROBLEMS_TSP_HPP

#include <memory>
#include <vector>
#include <cmath>

#include "physics/SA.hpp"

namespace physics
{
namespace SA
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
class TSS: public Solution
{
public:
    std::vector<int> tour;
};
using TSSPtr = std::shared_ptr<TSS>;

// Traveling Salesman Problem
class TSP: public Problem
{
public:
    TSP() = delete;
    explicit TSP(const Cities&);
    
    SolutionPtr generateInitialSolution() override;
    SolutionPtr generateNewSolution(SolutionPtr) override;
    float evaluateSolution(SolutionPtr) override;
    
    Cities mCities;
};
using TSPPtr = std::shared_ptr<TSP>;

} // namespace SA
} // namespace physics

#endif // MHAC_PROBLEMS_TSP_HPP

#ifndef MHAC_PHYSICS_TS_HPP
#define MHAC_PHYSICS_TS_HPP

#include "common.hpp"

namespace math
{
namespace TS
{

class TabuSearch
{
public:
    TabuSearch() = delete;
    explicit TabuSearch(common::ProblemPtr);
    TabuSearch(const TabuSearch&) = delete;
    TabuSearch(TabuSearch&&) = delete;
    TabuSearch& operator=(const TabuSearch&) = delete;
    TabuSearch& operator=(TabuSearch&&) = delete;
    virtual ~TabuSearch() = default;

    common::SolutionPtr solve(int iterations, int maxTabuListSize, int neighborhoodSize);

private:
    bool inTabuList(const common::SolutionPtr&);

    common::SolutionVec mTabuList;
    common::ProblemPtr mProblem;
};

} // namespace TS
} // namespace math

#endif // MHAC_PHYSICS_TS_HPP
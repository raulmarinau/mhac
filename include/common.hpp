#ifndef MHAC_COMMON_HPP
#define MHAC_COMMON_HPP

#include <vector>
#include <memory>

#include <pybind11/pybind11.h>

namespace common
{

class Solution
{
public:
    virtual ~Solution() = default;
    virtual bool isEqual(const Solution&) const = 0;
    virtual int getSize() const { return 0; }
    float cost = 0;
};
using SolutionPtr = std::shared_ptr<Solution>;
using SolutionVec = std::vector<SolutionPtr>;

class PySolution : public Solution
{
public:
    using Solution::Solution;

    bool isEqual(const Solution& sol) const override {
        PYBIND11_OVERRIDE_PURE(bool, Solution, isEqual, sol);
    }
};
using PySolutionPtr = std::shared_ptr<PySolution>;

class Problem
{
public:
    Problem() = default;
    Problem(const Problem&) = delete;
    Problem(Problem&&) = delete;
    Problem& operator=(const Problem&) = delete;
    Problem& operator=(Problem&&) = delete;
    virtual ~Problem() = default;

    virtual SolutionPtr generateInitialSolution() = 0;
    virtual SolutionPtr generateNewSolution(SolutionPtr) = 0;
    virtual float evaluateSolution(SolutionPtr) = 0;
};
using ProblemPtr = std::shared_ptr<Problem>;

class PyProblem : virtual public Problem
{
public:
    using Problem::Problem;

    SolutionPtr generateInitialSolution() override { 
        PYBIND11_OVERRIDE_PURE(SolutionPtr, Problem, generateInitialSolution);
    }
    SolutionPtr generateNewSolution(SolutionPtr sol) override {
        PYBIND11_OVERRIDE_PURE(SolutionPtr, Problem, generateNewSolution, sol);
    }
    float evaluateSolution(SolutionPtr sol) override {
        PYBIND11_OVERRIDE_PURE(float, Problem, evaluateSolution, sol);
    }
};
using PyProblemPtr = std::shared_ptr<PyProblem>;

} // namespace common

PYBIND11_MAKE_OPAQUE(common::SolutionVec);

#endif // MHAC_COMMON_HPP

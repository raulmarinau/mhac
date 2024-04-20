#ifndef MHAC_PHYSICS_SA_HPP
#define MHAC_PHYSICS_SA_HPP

/**
 * Need to establish:
 *  1. solution encoding
 *  2. local search optimization
 *  3. acceptance probability
 *  4. cooling schedule
 * 
 * pseudocode basic algorithm
 *      S = initial configuration
 *      T = initial temperature
 *      repeat
 *          S' = perturb(S)
 *          if accept(S, S', T) then S=S'
 *          T = update(T)
 *      until stopping condition
 * 
 * perturb(S)     - dependent on the problem to be solved
 * accept(S,S',T) - dependent on the quality loss from S to S'
 * 
 * pseudocode where acceptance is the Boltzmann distribution P(S’|S)=min\{1,\exp(-(cost(S’)-cost(S))/T(k)\}
 *      bool accept(S, S', T)
 *          if rand(0,1) < exp(-(f(S')-f(S))/T) then return true
 *          else return false
 * 
 * pseudocode temperature update
 *      update(T)
 *      T(k+1) = T(0)/log(k+c), where c is a constant
 *      T(k+1) = T(0)/k
 *      T(k+1) = aT(k), where a is close to 1 (eg. 0,99)
*/

#include <memory>

#include <pybind11/pybind11.h>

namespace physics
{
namespace SA
{

class Solution
{
public:
    virtual ~Solution() = default;
};
using SolutionPtr = std::shared_ptr<Solution>;


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

class PyProblem : public Problem
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


class SimulatedAnnealing
{
public:
    SimulatedAnnealing() = delete;
    explicit SimulatedAnnealing(ProblemPtr);
    SimulatedAnnealing(const SimulatedAnnealing&) = delete;
    SimulatedAnnealing(SimulatedAnnealing&&) = delete;
    SimulatedAnnealing& operator=(const SimulatedAnnealing&) = delete;
    SimulatedAnnealing& operator=(SimulatedAnnealing&&) = delete;
    virtual ~SimulatedAnnealing() = default;

    void solve(float maxT, float minT);
    SolutionPtr getSolution();

private:
    float updateTemp(float T);
    bool accept(float currCost, float newCost, float T);

    SolutionPtr mSolution;
    ProblemPtr mProblem;
};

} // namespace SA
} // namespace physics

#endif // MHAC_PHYSICS_SA_HPP

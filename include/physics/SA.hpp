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

#include <vector>
#include <cmath>

namespace physics
{

using Solution = std::vector<int>;

class Problem
{
public:
    Problem() = default;
    Problem(const Problem&) = delete;
    Problem(Problem&&) = delete;
    Problem& operator=(const Problem&) = delete;
    Problem& operator=(Problem&&) = delete;
    virtual ~Problem() = default;

    virtual Solution generateInitialSolution() = 0;
    virtual Solution generateNewSolution(const Solution&) = 0;
    virtual float evaluateSolution(const Solution&) = 0;
};

// TODO: consider moving to an example ns
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

class TSP: public Problem
{
public:
    TSP() = delete;
    explicit TSP(const Cities&);
    
    Solution generateInitialSolution() override;
    Solution generateNewSolution(const Solution&) override;
    float evaluateSolution(const Solution&) override;
    
    Cities mCities;
};
// TODO: consider moving to an example ns

class SimulatedAnnealing
{
public:
    SimulatedAnnealing() = delete;
    explicit SimulatedAnnealing(Problem*);
    SimulatedAnnealing(const SimulatedAnnealing&) = delete;
    SimulatedAnnealing(SimulatedAnnealing&&) = delete;
    SimulatedAnnealing& operator=(const SimulatedAnnealing&) = delete;
    SimulatedAnnealing& operator=(SimulatedAnnealing&&) = delete;
    virtual ~SimulatedAnnealing() = default;

    void solve(float maxT, float minT);
    Solution getSolution();

private:
    float updateTemp(float T);
    bool accept(float currCost, float newCost, float T);

    Solution mSol;
    Problem* mProblemType;
};
    
} // namespace physics


#endif // MHAC_PHYSICS_SA_HPP
#ifndef PHYSICS_SA
#define PHYSICS_SA

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

namespace physics
{

using Solution = int;

class SimulatedAnnealing
{
public:
    SimulatedAnnealing() = default;
    SimulatedAnnealing(const SimulatedAnnealing&) = delete;
    SimulatedAnnealing(SimulatedAnnealing&&) = delete;
    SimulatedAnnealing& operator=(const SimulatedAnnealing&) = delete;
    SimulatedAnnealing& operator=(SimulatedAnnealing&&) = delete;
    virtual ~SimulatedAnnealing() = default;

    void solve();
    Solution getSolution();

private:
    Solution s;
};
    
} // namespace physics


#endif // PHYSICS_SA
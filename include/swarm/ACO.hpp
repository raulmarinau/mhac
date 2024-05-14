#ifndef MHAC_SWARM_ACO_HPP
#define MHAC_SWARM_ACO_HPP

#include <memory>
#include <vector>

#include "common.hpp"
#include "pybind11/pybind11.h"

namespace swarm
{
namespace ACO
{

class PheromoneMatrix
{
public:
    PheromoneMatrix(int size, float initialValue);

    float& operator()(int i, int j) {
        return matrix_[i][j];
    }

    const float& operator()(int i, int j) const {
        return matrix_[i][j];
    }

    int getSize() const {
        return size_;
    }

private:
    int size_;
    std::vector<std::vector<float>> matrix_;
};
using PheromoneMatrixPtr = std::shared_ptr<PheromoneMatrix>;


class Problem : virtual public common::Problem
{
public:
    virtual void updateAntPath(common::SolutionPtr& ant, int node, PheromoneMatrixPtr pm, float alpha, float beta) = 0;
    virtual void updatePheromoneMatrix(common::SolutionPtr ant, PheromoneMatrixPtr& pm, float rho) = 0;
};
using ProblemPtr = std::shared_ptr<Problem>;

class PyProblem : public Problem, public common::PyProblem
{
public:
    using Problem::Problem;

    void updateAntPath(common::SolutionPtr& ant, int node, PheromoneMatrixPtr pm, float alpha, float beta) override
    {
        PYBIND11_OVERRIDE_PURE(void, Problem, updateAntPath, ant, node, pm, alpha, beta);
    }

    void updatePheromoneMatrix(common::SolutionPtr ant, PheromoneMatrixPtr& pm, float rho) override
    {
        PYBIND11_OVERRIDE_PURE(void, Problem, updatePheromoneMatrix, ant, pm, rho);
    }
};
using PyProblemPtr = std::shared_ptr<PyProblem>;


class AntColonyOptimization
{
public:
    AntColonyOptimization() = delete;
    explicit AntColonyOptimization(ProblemPtr);
    AntColonyOptimization(const AntColonyOptimization&) = delete;
    AntColonyOptimization(AntColonyOptimization&&) = delete;
    AntColonyOptimization& operator=(const AntColonyOptimization&) = delete;
    AntColonyOptimization& operator=(AntColonyOptimization&&) = delete;
    virtual ~AntColonyOptimization() = default;

    common::SolutionPtr solve(int generations, int colonySize, float alpha, float beta, float rho);

private:
    ProblemPtr mProblem;
    PheromoneMatrixPtr mPheromoneMatrix;
};

} // namespace ACO
} // namespace swarm

#endif // MHAC_SWARM_ACO_HPP
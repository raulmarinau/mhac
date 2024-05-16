#include "logger/logger.hpp"
#include "random/random.hpp"

#include "problems/JSS.hpp"
#include <string>

namespace problems
{
namespace jss
{

bool JSSS::isEqual(const common::Solution & other) const
{
    const JSSS* otherJSSS = dynamic_cast<const JSSS*>(&other);
    return this->schedule == otherJSSS->schedule;
}

int JSSS::getSize() const
{
    return this->schedule.size();
}

std::string JSSS::print()
{
    std::string s;
    for (int i = 0; i < (int) schedule.size(); i++)
    {
        s += std::to_string(schedule[i]) + " ";
    }
    return s;
}

JSSP::JSSP(const TimeMatrix& products)
{
    this->products = products;
}

float JSSP::evaluateSolution(common::SolutionPtr sol)
{
    JSSSPtr sol_sequence = std::dynamic_pointer_cast<JSSS>(sol);
    
    int num_products = products.size();
    int num_machines = products[0].size();
    TimeMatrix completion_times(num_products, ProductVec(num_machines, 0));
    int total_completition_time = 0;

    for (int productIndex = 0; productIndex < num_products; productIndex++) {
        int product = sol_sequence->schedule[productIndex];

        for (int machine = 0; machine < num_machines; machine++) {
            if (productIndex == 0) {
                completion_times[productIndex][machine] = this->products[product][machine];
            }
            else {
                if (machine == 0) {
                    completion_times[productIndex][machine] =
                        completion_times[productIndex-1][machine] + this->products[product][machine];
                }
                else {
                    completion_times[productIndex][machine] = this->products[product][machine] +
                        std::max(completion_times[productIndex][machine-1], completion_times[productIndex-1][machine]);
                }
            }

            if (machine == num_machines - 1) {
                total_completition_time += completion_times[productIndex][machine];
            }
        }
    }

    globalLogger->debug("Schedule: " + sol_sequence->print());

    globalLogger->debug("Completition times:");

    for (int i = 0; i < num_products; i++) {
        std::string row;
        for (int j = 0; j < num_machines; j++) {
            row += std::to_string(completion_times[i][j]) + " ";
        }
        globalLogger->debug(row);
    }

    globalLogger->debug("Total cost: " + std::to_string(total_completition_time));

    return total_completition_time;
}

common::SolutionPtr JSSP::generateInitialSolution()
{
    JSSSPtr tss = std::make_shared<JSSS>();
    tss->schedule = mhac_random::sample(products.size(), products.size());
    return tss;
}

common::SolutionPtr JSSP::generateNewSolution(common::SolutionPtr initialSol)
{
    JSSSPtr jssInitial = std::dynamic_pointer_cast<JSSS>(initialSol);
    JSSSPtr jssNew = std::make_shared<JSSS>();
    jssNew->schedule = jssInitial->schedule;

    std::vector<int> indexes = mhac_random::sample(jssInitial->getSize(), 2);
    int i = indexes[0];
    int j = indexes[1];

    if (i > j)
        std::swap(i, j);

    // for (int k = 0; k < (j-i+1) / 2; k++)
    //     std::swap(jssNew->schedule[i+k], jssNew->schedule[j-k]);
    
    std::swap(jssNew->schedule[i], jssNew->schedule[j]);

    return jssNew;
}

} // namespace jss
}// namespace problems

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <chrono>

#include "random/random.hpp"

namespace mhac_random
{

float random(float start, float end)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr(start, end);  // [start, end]

    return distr(gen);
}

std::vector<int> sample(int range_size, int count)
{
    std::vector<int> numbers(range_size);

    // Fill numbers with 0, 1, ..., N-1
    std::iota(numbers.begin(), numbers.end(), 0);

    // Obtain a time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // Shuffle numbers using the default random engine seeded with the above seed
    std::shuffle(numbers.begin(), numbers.end(), std::default_random_engine(seed));

    numbers.resize(count);

    return numbers;
}

} // namespace mhac_random
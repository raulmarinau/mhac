#ifndef MHAC_RANDOM_HPP
#define MHAC_RANDOM_HPP

#include <vector>

namespace mhac_random
{

float random(float start = 0, float end = 1);
std::vector<int> sample(int range_size, int count);

} // namespace mhac_random

#endif // MHAC_RANDOM_HPP
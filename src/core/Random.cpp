#include "core/Random.hpp"

#include <stdexcept>

namespace taperot {

Random::Random(std::uint32_t seed)
    : seed_(seed)
    , engine_(seed)
{
}

std::uint32_t Random::seed() const noexcept
{
    return seed_;
}

double Random::unit()
{
    return std::uniform_real_distribution<double>(0.0, 1.0)(engine_);
}

int Random::integer(int minInclusive, int maxInclusive)
{
    if (minInclusive > maxInclusive) {
        throw std::invalid_argument("invalid random integer range");
    }

    return std::uniform_int_distribution<int>(minInclusive, maxInclusive)(engine_);
}

} // namespace taperot

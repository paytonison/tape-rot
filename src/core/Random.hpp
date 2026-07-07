#pragma once

#include <cstdint>
#include <random>

namespace taperot {

class Random {
public:
    explicit Random(std::uint32_t seed);

    std::uint32_t seed() const noexcept;
    double unit();
    int integer(int minInclusive, int maxInclusive);

private:
    std::uint32_t seed_;
    std::mt19937 engine_;
};

} // namespace taperot

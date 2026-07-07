#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <string_view>

namespace taperot::effects::detail {

inline void validateUnitInterval(double value, std::string_view name)
{
    if (!std::isfinite(value) || value < 0.0 || value > 1.0) {
        throw std::invalid_argument(std::string(name) + " must be from 0.0 to 1.0");
    }
}

inline float clampUnit(float value)
{
    return std::clamp(value, 0.0F, 1.0F);
}

inline float clampChroma(float value)
{
    return std::clamp(value, -1.0F, 1.0F);
}

inline std::size_t pixelIndex(std::size_t x, std::size_t y, std::size_t width)
{
    return y * width + x;
}

} // namespace taperot::effects::detail

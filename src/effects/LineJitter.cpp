#include "effects/LineJitter.hpp"

#include "effects/EffectUtil.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace taperot::effects {
namespace {

void shiftPlaneLine(std::vector<float>& plane, const std::vector<float>& source,
    std::size_t width, std::size_t y, int offset)
{
    for (std::size_t x = 0; x < width; ++x) {
        const auto sourceX = static_cast<std::size_t>(
            std::clamp(static_cast<int>(x) - offset, 0, static_cast<int>(width) - 1));
        plane[detail::pixelIndex(x, y, width)] = source[detail::pixelIndex(sourceX, y, width)];
    }
}

} // namespace

LineJitter::LineJitter(LineJitterParameters parameters)
    : parameters_(parameters)
{
    detail::validateUnitInterval(parameters_.strength, "line jitter strength");
    if (parameters_.maxOffset < 0) {
        throw std::invalid_argument("line jitter max offset must not be negative");
    }
}

std::string_view LineJitter::name() const noexcept
{
    return "line jitter";
}

void LineJitter::apply(Frame& frame, Random& random)
{
    if (frame.empty() || parameters_.strength == 0.0 || parameters_.maxOffset == 0 ||
        frame.width() < 2) {
        return;
    }

    const int frameLimitedMax = static_cast<int>(std::min<std::size_t>(
        frame.width() - 1, static_cast<std::size_t>(parameters_.maxOffset)));
    const int maxOffset =
        std::max(1, static_cast<int>(std::ceil(parameters_.strength * frameLimitedMax)));

    const std::vector<float> sourceLuma = frame.luma();
    const std::vector<float> sourceBlue = frame.chromaBlue();
    const std::vector<float> sourceRed = frame.chromaRed();
    bool shiftedAnyLine = false;

    for (std::size_t y = 0; y < frame.height(); ++y) {
        int offset = random.integer(-maxOffset, maxOffset);
        if (offset == 0 && frame.height() > 1 && y % 4 == 0) {
            offset = random.integer(0, 1) == 0 ? -1 : 1;
        }

        shiftedAnyLine = shiftedAnyLine || offset != 0;
        shiftPlaneLine(frame.luma(), sourceLuma, frame.width(), y, offset);
        shiftPlaneLine(frame.chromaBlue(), sourceBlue, frame.width(), y, offset);
        shiftPlaneLine(frame.chromaRed(), sourceRed, frame.width(), y, offset);
    }

    if (!shiftedAnyLine) {
        const std::size_t y = frame.height() / 2;
        shiftPlaneLine(frame.luma(), sourceLuma, frame.width(), y, 1);
        shiftPlaneLine(frame.chromaBlue(), sourceBlue, frame.width(), y, 1);
        shiftPlaneLine(frame.chromaRed(), sourceRed, frame.width(), y, 1);
    }
}

} // namespace taperot::effects

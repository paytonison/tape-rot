#include "effects/HeadSwitchNoise.hpp"

#include "effects/EffectUtil.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace taperot::effects {
namespace {

float signedNoise(Random& random, double amplitude)
{
    return static_cast<float>((random.unit() * 2.0 - 1.0) * amplitude);
}

float shiftedSample(const std::vector<float>& source, std::size_t width, std::size_t y,
    std::size_t x, int offset)
{
    const auto sourceX = static_cast<std::size_t>(
        std::clamp(static_cast<int>(x) - offset, 0, static_cast<int>(width) - 1));
    return source[detail::pixelIndex(sourceX, y, width)];
}

} // namespace

HeadSwitchNoise::HeadSwitchNoise(HeadSwitchNoiseParameters parameters)
    : parameters_(parameters)
{
    detail::validateUnitInterval(parameters_.strength, "head-switch noise strength");
    if (!std::isfinite(parameters_.bandFraction) || parameters_.bandFraction <= 0.0 ||
        parameters_.bandFraction > 0.50) {
        throw std::invalid_argument("head-switch noise band fraction must be from 0.0 to 0.5");
    }
}

std::string_view HeadSwitchNoise::name() const noexcept
{
    return "head-switch noise";
}

void HeadSwitchNoise::apply(Frame& frame, Random& random)
{
    if (frame.empty() || parameters_.strength == 0.0) {
        return;
    }

    const std::size_t bandHeight = std::max<std::size_t>(
        1, static_cast<std::size_t>(std::ceil(frame.height() * parameters_.bandFraction)));
    const std::size_t startY = frame.height() > bandHeight ? frame.height() - bandHeight : 0;
    const int maxTear = frame.width() < 2
        ? 0
        : std::max(1, static_cast<int>(std::ceil(parameters_.strength *
                          std::min<std::size_t>(frame.width() - 1, 10))));

    const std::vector<float> sourceLuma = frame.luma();
    const std::vector<float> sourceBlue = frame.chromaBlue();
    const std::vector<float> sourceRed = frame.chromaRed();

    for (std::size_t y = startY; y < frame.height(); ++y) {
        const double bandPosition = bandHeight == 1
            ? 1.0
            : static_cast<double>(y - startY) / static_cast<double>(bandHeight - 1);
        const int offset = maxTear == 0 ? 0 : random.integer(-maxTear, maxTear);
        const float brightnessKick =
            static_cast<float>((0.08 + 0.18 * bandPosition) * parameters_.strength) +
            signedNoise(random, 0.12 * parameters_.strength);

        for (std::size_t x = 0; x < frame.width(); ++x) {
            const auto index = detail::pixelIndex(x, y, frame.width());
            const float lumaNoise = signedNoise(random, 0.10 * parameters_.strength);
            frame.luma()[index] = detail::clampUnit(
                shiftedSample(sourceLuma, frame.width(), y, x, offset) + brightnessKick + lumaNoise);

            const float chromaGain = static_cast<float>(1.0 - 0.45 * parameters_.strength);
            frame.chromaBlue()[index] = detail::clampChroma(
                shiftedSample(sourceBlue, frame.width(), y, x, offset) * chromaGain +
                signedNoise(random, 0.05 * parameters_.strength));
            frame.chromaRed()[index] = detail::clampChroma(
                shiftedSample(sourceRed, frame.width(), y, x, offset) * chromaGain +
                signedNoise(random, 0.05 * parameters_.strength));
        }
    }
}

} // namespace taperot::effects

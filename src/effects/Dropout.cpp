#include "effects/Dropout.hpp"

#include "effects/EffectUtil.hpp"

#include <algorithm>
#include <cmath>

namespace taperot::effects {
namespace {

void applyDropoutPixel(Frame& frame, std::size_t index, float targetLuma, float damage)
{
    frame.luma()[index] = detail::clampUnit(frame.luma()[index] * (1.0F - damage) +
        targetLuma * damage);

    const float chromaLoss = 1.0F - (0.80F * damage);
    frame.chromaBlue()[index] = detail::clampChroma(frame.chromaBlue()[index] * chromaLoss);
    frame.chromaRed()[index] = detail::clampChroma(frame.chromaRed()[index] * chromaLoss);
}

} // namespace

Dropout::Dropout(DropoutParameters parameters)
    : parameters_(parameters)
{
    detail::validateUnitInterval(parameters_.strength, "dropout strength");
}

std::string_view Dropout::name() const noexcept
{
    return "dropouts";
}

void Dropout::apply(Frame& frame, Random& random)
{
    if (frame.empty() || parameters_.strength == 0.0) {
        return;
    }

    const int width = static_cast<int>(frame.width());
    const int height = static_cast<int>(frame.height());
    const int streakCount =
        std::max(1, static_cast<int>(std::ceil(parameters_.strength * height * 0.30)));
    const int maxLength = std::max(
        1, static_cast<int>(std::ceil(width * (0.20 + 0.45 * parameters_.strength))));
    const int maxThickness =
        std::max(1, static_cast<int>(std::ceil(1.0 + parameters_.strength * 2.0)));
    const float damage = static_cast<float>(0.45 + 0.50 * parameters_.strength);

    for (int streak = 0; streak < streakCount; ++streak) {
        const int y = random.integer(0, height - 1);
        const int x = random.integer(0, width - 1);
        const int length = random.integer(1, maxLength);
        const int thickness = random.integer(1, maxThickness);
        const float targetLuma = random.unit() < 0.75 ? 0.96F : 0.08F;

        for (int row = 0; row < thickness && y + row < height; ++row) {
            for (int column = 0; column < length && x + column < width; ++column) {
                // Tape dropouts are ragged horizontal signal losses, not full-frame static.
                if ((row != 0 || column != 0) && random.unit() < 0.12) {
                    continue;
                }

                const auto index = detail::pixelIndex(
                    static_cast<std::size_t>(x + column), static_cast<std::size_t>(y + row),
                    frame.width());
                applyDropoutPixel(frame, index, targetLuma, damage);
            }
        }
    }
}

} // namespace taperot::effects

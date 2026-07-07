#include "effects/ChromaDegrade.hpp"

#include "effects/EffectUtil.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace taperot::effects {
namespace {

void smearChromaPlane(std::vector<float>& plane, std::size_t width, std::size_t height,
    int radius, float chromaGain)
{
    const std::vector<float> source = plane;

    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            float total = 0.0F;
            int samples = 0;

            for (int offset = -radius; offset <= radius; ++offset) {
                const auto sourceX = static_cast<std::size_t>(
                    std::clamp(static_cast<int>(x) + offset, 0, static_cast<int>(width) - 1));
                total += source[detail::pixelIndex(sourceX, y, width)];
                ++samples;
            }

            const float softened = total / static_cast<float>(samples);
            plane[detail::pixelIndex(x, y, width)] = detail::clampChroma(softened * chromaGain);
        }
    }
}

} // namespace

ChromaDegrade::ChromaDegrade(ChromaDegradeParameters parameters)
    : parameters_(parameters)
{
    detail::validateUnitInterval(parameters_.strength, "chroma degradation strength");
}

std::string_view ChromaDegrade::name() const noexcept
{
    return "chroma degradation";
}

void ChromaDegrade::apply(Frame& frame, Random&)
{
    if (frame.empty() || parameters_.strength == 0.0) {
        return;
    }

    const int radius = std::max(1, static_cast<int>(std::ceil(parameters_.strength * 4.0)));
    const float chromaGain = static_cast<float>(1.0 - 0.35 * parameters_.strength);

    // Consumer analog tape kept brightness detail better than color detail, so
    // this stage smears and weakens U/V planes while leaving luma untouched.
    smearChromaPlane(frame.chromaBlue(), frame.width(), frame.height(), radius, chromaGain);
    smearChromaPlane(frame.chromaRed(), frame.width(), frame.height(), radius, chromaGain);
}

} // namespace taperot::effects

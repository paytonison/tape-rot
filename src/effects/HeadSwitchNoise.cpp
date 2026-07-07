#include "effects/HeadSwitchNoise.hpp"

namespace taperot::effects {

std::string_view HeadSwitchNoise::name() const noexcept
{
    return "head-switch noise";
}

void HeadSwitchNoise::apply(Frame&, Random&)
{
    // Future work: corrupt the lower portion of the frame where helical-scan
    // tape machines switch playback heads, producing a localized unstable band.
}

} // namespace taperot::effects

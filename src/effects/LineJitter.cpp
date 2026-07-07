#include "effects/LineJitter.hpp"

namespace taperot::effects {

std::string_view LineJitter::name() const noexcept
{
    return "line jitter";
}

void LineJitter::apply(Frame&, Random&)
{
    // Future work: shift individual scanlines horizontally to model time-base
    // instability from imperfect tape transport and playback synchronization.
}

} // namespace taperot::effects

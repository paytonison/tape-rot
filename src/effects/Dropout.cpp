#include "effects/Dropout.hpp"

namespace taperot::effects {

std::string_view Dropout::name() const noexcept
{
    return "dropouts";
}

void Dropout::apply(Frame&, Random&)
{
    // Future work: create short scanline-oriented signal losses from tape wear,
    // dust, or damaged magnetic particles rather than generic image noise.
}

} // namespace taperot::effects

#include "effects/ChromaDegrade.hpp"

namespace taperot::effects {

std::string_view ChromaDegrade::name() const noexcept
{
    return "chroma degradation";
}

void ChromaDegrade::apply(Frame&, Random&)
{
    // Future work: soften and destabilize chroma planes more than luma because
    // analog tape formats retained brightness detail better than color detail.
}

} // namespace taperot::effects

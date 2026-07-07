#include "effects/EffectPipeline.hpp"

#include <stdexcept>

namespace taperot::effects {

void EffectPipeline::add(std::unique_ptr<Effect> effect)
{
    if (!effect) {
        throw std::invalid_argument("cannot add a null effect to the pipeline");
    }

    effects_.push_back(std::move(effect));
}

void EffectPipeline::apply(Frame& frame, Random& random) const
{
    for (const auto& effect : effects_) {
        effect->apply(frame, random);
    }
}

bool EffectPipeline::empty() const noexcept
{
    return effects_.empty();
}

std::size_t EffectPipeline::size() const noexcept
{
    return effects_.size();
}

} // namespace taperot::effects

#pragma once

#include "effects/Effect.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace taperot::effects {

class EffectPipeline {
public:
    void add(std::unique_ptr<Effect> effect);
    void apply(Frame& frame, Random& random) const;

    bool empty() const noexcept;
    std::size_t size() const noexcept;

private:
    std::vector<std::unique_ptr<Effect>> effects_;
};

} // namespace taperot::effects

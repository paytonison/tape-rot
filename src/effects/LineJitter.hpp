#pragma once

#include "effects/Effect.hpp"

namespace taperot::effects {

class LineJitter final : public Effect {
public:
    std::string_view name() const noexcept override;
    void apply(Frame& frame, Random& random) override;
};

} // namespace taperot::effects

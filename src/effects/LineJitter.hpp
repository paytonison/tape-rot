#pragma once

#include "effects/Effect.hpp"

namespace taperot::effects {

struct LineJitterParameters {
    double strength = 0.5;
    int maxOffset = 8;
};

class LineJitter final : public Effect {
public:
    explicit LineJitter(LineJitterParameters parameters = LineJitterParameters{});

    std::string_view name() const noexcept override;
    void apply(Frame& frame, Random& random) override;

private:
    LineJitterParameters parameters_;
};

} // namespace taperot::effects

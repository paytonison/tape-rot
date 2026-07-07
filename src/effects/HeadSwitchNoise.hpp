#pragma once

#include "effects/Effect.hpp"

namespace taperot::effects {

struct HeadSwitchNoiseParameters {
    double strength = 0.5;
    double bandFraction = 0.14;
};

class HeadSwitchNoise final : public Effect {
public:
    explicit HeadSwitchNoise(HeadSwitchNoiseParameters parameters = HeadSwitchNoiseParameters{});

    std::string_view name() const noexcept override;
    void apply(Frame& frame, Random& random) override;

private:
    HeadSwitchNoiseParameters parameters_;
};

} // namespace taperot::effects

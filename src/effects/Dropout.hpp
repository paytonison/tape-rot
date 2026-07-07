#pragma once

#include "effects/Effect.hpp"

namespace taperot::effects {

struct DropoutParameters {
    double strength = 0.5;
};

class Dropout final : public Effect {
public:
    explicit Dropout(DropoutParameters parameters = DropoutParameters{});

    std::string_view name() const noexcept override;
    void apply(Frame& frame, Random& random) override;

private:
    DropoutParameters parameters_;
};

} // namespace taperot::effects

#pragma once

#include "effects/Effect.hpp"

namespace taperot::effects {

struct ChromaDegradeParameters {
    double strength = 0.5;
};

class ChromaDegrade final : public Effect {
public:
    explicit ChromaDegrade(ChromaDegradeParameters parameters = ChromaDegradeParameters{});

    std::string_view name() const noexcept override;
    void apply(Frame& frame, Random& random) override;

private:
    ChromaDegradeParameters parameters_;
};

} // namespace taperot::effects

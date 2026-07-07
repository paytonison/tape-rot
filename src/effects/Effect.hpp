#pragma once

#include "core/Frame.hpp"
#include "core/Random.hpp"

#include <string_view>

namespace taperot::effects {

class Effect {
public:
    virtual ~Effect() = default;

    virtual std::string_view name() const noexcept = 0;
    virtual void apply(Frame& frame, Random& random) = 0;
};

} // namespace taperot::effects

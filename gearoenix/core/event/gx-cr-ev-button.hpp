#ifndef GEAROENIX_CORE_EVENT_BUTTON_BUTTON_HPP
#define GEAROENIX_CORE_EVENT_BUTTON_BUTTON_HPP
#include "../../math/gx-math-vector-2d.hpp"
#include "../../platform/gx-plt-key.hpp"

namespace gearoenix::core::event::button {
struct Keyboard final {
    GX_GET_CVAL_PRV(platform::key::Action, action);
    GX_GET_CVAL_PRV(platform::key::Id, key);

public:
    Keyboard(const platform::key::Action action, const platform::key::Id key)
        : action(action)
        , key(key)
    {
    }
};

struct Mouse final {
    GX_GET_CVAL_PRV(platform::key::Action, action);
    GX_GET_CVAL_PRV(platform::key::Id, key);
    GX_GET_CREF_PRV(math::Vec2<double>, position_normalised);
    GX_GET_CREF_PRV(math::Vec2<double>, position);

public:
    Mouse(
        const platform::key::Action action,
        const platform::key::Id key,
        const math::Vec2<double>& position_normalised,
        const math::Vec2<double>& position)
        : action(action)
        , key(key)
        , position_normalised(position_normalised)
        , position(position)
    {
    }
};

struct MouseScroll final {
    GX_GET_REFC_PRV(math::Vec2<double>, direction);

public:
    explicit MouseScroll(const math::Vec2<double>& direction)
        : direction(direction)
    {
    }
};

}
#endif
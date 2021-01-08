#ifndef GEAROENIX_CORE_EVENT_EVENT_HPP
#define GEAROENIX_CORE_EVENT_EVENT_HPP
#include "gx-cr-ev-button.hpp"
#include "gx-cr-ev-gesture.hpp"
#include "gx-cr-ev-id.hpp"
#include "gx-cr-ev-movement.hpp"
#include "gx-cr-ev-system.hpp"
#include "gx-cr-ev-touch.hpp"
#include <utility>
#include <variant>

namespace gearoenix::core::event {
struct Data {
    typedef std::variant<
        button::KeyboardData,
        button::MouseData,
        button::MouseScroll,
        gesture::Click,
        gesture::Drag2D,
        gesture::Scale,
        gesture::MouseDrag,
        gesture::TouchClick,
        gesture::TouchDrag,
        gesture::TouchScale,
        movement::Base2D,
        platform::WindowSizeChangeData,
        touch::Data,
        int>
        DataVar;

    GX_GET_CVAL_PRV(Id, source)
    GX_GET_REFC_PRV(DataVar, data)

    Data(const Id source, DataVar data) noexcept
        : source(source)
        , data(std::move(data))
    {
    }
};
}
#endif

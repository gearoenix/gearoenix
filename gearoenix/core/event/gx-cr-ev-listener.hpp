#pragma once
#include "gx-cr-ev-event.hpp"

namespace gearoenix::core::event {
struct Listener {
    enum struct Response {
        Erase,
        Continue,
        Discontinue,
    };

    virtual ~Listener() = default;
    /// If it returns true no more process on event will happen
    [[nodiscard]] virtual Response on_event(const Data& event_data) = 0;
};
}
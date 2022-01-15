#ifndef GEAROENIX_CORE_EVENT_LISTENER_HPP
#define GEAROENIX_CORE_EVENT_LISTENER_HPP
#include "gx-cr-ev-event.hpp"

namespace gearoenix::core::event {
struct Listener {
    enum struct Response {
        Erase,
        Continue,
        Discontinue,
    };

public:
    virtual ~Listener() noexcept = default;
    /// If it returns true no more process on event will happen
    [[nodiscard]] virtual Response on_event(const Data& event_data) noexcept = 0;
};
}
#endif
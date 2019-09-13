#ifndef GEAROENIX_CORE_EVENT_LISTNER_HPP
#define GEAROENIX_CORE_EVENT_LISTNER_HPP
#include "cr-ev-event.hpp"
namespace gearoenix::core::event {
class Listner {
public:
    /// If it returns true no more process on event will happen
    virtual bool on_event(const Data& event_data) noexcept = 0;
};
}
#endif
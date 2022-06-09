#ifndef GEAROENIX_CORE_EVENT_ENGINE_HPP
#define GEAROENIX_CORE_EVENT_ENGINE_HPP
#include "gx-cr-ev-event.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <tuple>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::core::event {
struct Listener;
struct Engine final {
private:
    boost::container::flat_set<std::tuple<Id, double, Listener*>> events_id_priority_listeners;

public:
    Engine() noexcept;
    ~Engine() noexcept;
    Engine(const Engine&) = delete;
    void add_listener(Id event_id, Listener* listener, double priority = 0.0) noexcept;
    void remove_listener(Id event_id, double priority, Listener* listener) noexcept;
    void remove_listener(Id event_id, Listener* listener) noexcept;
    void remove_listener(Listener* listener) noexcept;
    void broadcast(const Data& event_data) noexcept;
};
}
#endif
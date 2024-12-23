#pragma once
#include "gx-cr-ev-event.hpp"
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
    Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    void add_listener(Id event_id, Listener* listener, double priority = 0.0);
    void remove_listener(Id event_id, double priority, Listener* listener);
    void remove_listener(Id event_id, Listener* listener);
    void remove_listener(Listener* listener);
    void broadcast(const Data& event_data);
};
}
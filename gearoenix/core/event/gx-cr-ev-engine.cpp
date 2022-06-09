#include "gx-cr-ev-engine.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-ev-listener.hpp"

gearoenix::core::event::Engine::Engine() noexcept { }

gearoenix::core::event::Engine::~Engine() noexcept
{
    if (!events_id_priority_listeners.empty())
        GX_UNEXPECTED;
}

void gearoenix::core::event::Engine::add_listener(const Id event_id, Listener* const listener, const double priority) noexcept
{
    events_id_priority_listeners.emplace(event_id, priority, listener);
}

void gearoenix::core::event::Engine::remove_listener(const Id event_id, const double priority, Listener* const listener) noexcept
{
    events_id_priority_listeners.erase(std::make_tuple(event_id, priority, listener));
}

void gearoenix::core::event::Engine::remove_listener(const Id event_id, Listener* const listener) noexcept
{
    for (
        auto search = events_id_priority_listeners.upper_bound(std::make_tuple(event_id, -std::numeric_limits<double>::min(), listener));
        std::get<0>(*search) == event_id;
        std::get<2>(*search) == listener ? (search = events_id_priority_listeners.erase(search)) : ++search)
        ;
}

void gearoenix::core::event::Engine::remove_listener(Listener* listener) noexcept
{
    for (
        auto search = events_id_priority_listeners.begin();
        search != events_id_priority_listeners.end();
        std::get<2>(*search) == listener ? (search = events_id_priority_listeners.erase(search)) : ++search)
        ;
}

void gearoenix::core::event::Engine::broadcast(const Data& event_data) noexcept
{
    bool keep_continuing = true;
    for (
        auto search = events_id_priority_listeners.upper_bound(std::make_tuple(event_data.get_source(), -std::numeric_limits<double>::min(), nullptr));
        keep_continuing && search != events_id_priority_listeners.end() && std::get<0>(*search) == event_data.get_source();) {
        switch (std::get<2>(*search)->on_event(event_data)) {
        case Listener::Response::Erase:
            search = events_id_priority_listeners.erase(search);
            break;
        case Listener::Response::Continue:
            ++search;
            break;
        case Listener::Response::Discontinue:
            keep_continuing = false;
            break;
        }
    }
}
#include "cr-ev-engine.hpp"
#include "../../system/sys-log.hpp"
#include "cr-ev-listner.hpp"
#include <functional>

void gearoenix::core::event::Engine::loop() noexcept
{
    while (state == State::RUNNING) {
        signaler.lock();
        decltype(events) es;
        {
            std::lock_guard<std::mutex> _l(events_guard);
            std::swap(events, es);
        }
        std::lock_guard<std::mutex> _l(listners_guard);
        for (const auto& e : es) {
            auto& ps = events_id_priority_listners[e.source];
            for (auto& p : ps) {
                auto& ls = p.second;
                for (auto& l : ls) {
                    if (l->on_event(e)) {
                        goto event_processed;
                    }
                }
            }
        event_processed:;
        }
    }
    state = State::TERMINATED;
}

gearoenix::core::event::Engine::Engine() noexcept
    : event_thread(std::bind(&Engine::loop, this))
{
}

gearoenix::core::event::Engine::~Engine() noexcept
{
    state = State::TERMINATING;
    while (state != State::TERMINATED)
        signaler.release();
    event_thread.join();
}

void gearoenix::core::event::Engine::add_listner(Id event_id, Real priority, Listner* const listner) noexcept
{
    std::lock_guard<std::mutex> _l(listners_guard);
    events_id_priority_listners[event_id][priority].insert(listner);
}

void gearoenix::core::event::Engine::remove_listner(Id event_id, Real priority, Listner* const listner) noexcept
{
    std::lock_guard<std::mutex> _l(listners_guard);
    events_id_priority_listners[event_id][priority].erase(listner);
}

void gearoenix::core::event::Engine::remove_listner(Id event_id, Listner* const listner) noexcept
{
    std::lock_guard<std::mutex> _l(listners_guard);
    auto& e = events_id_priority_listners[event_id];
    for (auto& p : e)
        p.second.erase(listner);
}

void gearoenix::core::event::Engine::remove_listner(Listner* const listner) noexcept
{
    std::lock_guard<std::mutex> _l(listners_guard);
    for (auto& e : events_id_priority_listners) {
        auto& ps = e.second;
        for (auto& p : ps)
            p.second.erase(listner);
    }
}

void gearoenix::core::event::Engine::braodcast(Data event_data) noexcept
{
    std::lock_guard<std::mutex> _l(events_guard);
    events.push_back(event_data);
    signaler.release();
}

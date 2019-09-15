#include "cr-ev-engine.hpp"
#include "../../system/sys-log.hpp"
#include "cr-ev-listner.hpp"
#include <functional>

constexpr gearoenix::core::Real CLICK_THRESHOLD = 0.2f;

void gearoenix::core::event::Engine::loop() noexcept
{
	state = State::Running;
    while (state == State::Running) {
        signaler.lock();
		if (state != State::Running) break;
        decltype(events) es;
        {
            std::lock_guard<std::mutex> _l(events_guard);
            std::swap(events, es);
        }
		if (state != State::Running) break;
        std::lock_guard<std::mutex> _l(listners_guard);
        for (const auto& e : es) {
			if (state != State::Running) break;
            auto& ps = events_id_priority_listners[e.source];
			if (state != State::Running) break;
            for (auto& p : ps) {
				if (state != State::Running) break;
                auto& ls = p.second;
				if (state != State::Running) break;
                for (auto& l : ls) {
					if (state != State::Running) break;
                    if (l->on_event(e)) {
						if (state != State::Running) break;
                        goto event_processed;
                    }
					if (state != State::Running) break;
                }
				if (state != State::Running) break;
            }
		event_processed:;
			if (state != State::Running) break;
        }
		if (state != State::Running) break;
    }
    state = State::Terminated;
}

gearoenix::core::event::Engine::Engine() noexcept
    : state(State::Running)
	, event_thread(std::bind(&Engine::loop, this))
{
}

gearoenix::core::event::Engine::~Engine() noexcept
{
    state = State::Terminating;
    while (state != State::Terminated)
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

void gearoenix::core::event::Engine::broadcast(Data event_data) noexcept
{
    std::lock_guard<std::mutex> _l(events_guard);
    events.push_back(event_data);
    signaler.release();
}

void gearoenix::core::event::Engine::set_mouse_position(const math::Vec2& p) noexcept
{
	mouse_movement.position = p;
	mouse_movement.update();
}

void gearoenix::core::event::Engine::set_mouse_movement(const math::Vec2& p) noexcept
{
	mouse_movement.previous_position = mouse_movement.position;
	mouse_movement.previous_time = mouse_movement.now_time;
	mouse_movement.position = p;
	mouse_movement.update();
	Data d;
	d.source = Id::MovementMouse;
	d.data = mouse_movement;
	broadcast(d);

    for (auto& ap : pressed_mouse_buttons_state) {
        auto& p = ap.second;
        auto dt = mouse_movement.now_time - p.start_time;

    }
}

void gearoenix::core::event::Engine::mouse_button(const button::MouseKeyId k, const button::MouseActionId a)
{
    button::MouseData bd = {};
    bd.action = a;
    bd.key = k;
    bd.position = mouse_movement.position;
    
    core::event::Data e = {};
    e.source = core::event::Id::ButtonMouse;
    e.data = bd;
    broadcast(e);

    if (a == button::MouseActionId::Press) {
        auto now = std::chrono::high_resolution_clock::now();
        auto& p = pressed_mouse_buttons_state[k];
        p = MouseButtonState{};
        p.starting = mouse_movement.position;
        p.previous = mouse_movement.position;
    }
    else if (a == button::MouseActionId::Release) {
        auto pi = pressed_mouse_buttons_state.find(k);
        if (pi == pressed_mouse_buttons_state.begin()) {
            auto now = std::chrono::high_resolution_clock::now();
            auto& p = pi->second;
            const std::chrono::duration<core::Real> dt = now - p.previous_time;
            const core::Real fdt = dt.count();
            if (dt.count() < CLICK_THRESHOLD) {
                bd.action = button::MouseActionId::Click;
                e.data = bd;
                broadcast(e);
            }
            pressed_mouse_buttons_state.erase(k);
        }
    }
}

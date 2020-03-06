#include "cr-ev-engine.hpp"
#include "../../system/sys-log.hpp"
#include "cr-ev-listener.hpp"
#include <functional>

constexpr gearoenix::core::Real CLICK_THRESHOLD = 0.2f;
constexpr gearoenix::core::Real MOUSE_DRAG_DISTANCE_THRESHOLD = 0.1f;

void gearoenix::core::event::Engine::loop() noexcept
{
    state = State::Running;
    while (state == State::Running) {
        signaler.lock();
        if (state != State::Running)
            break;
        decltype(events) es;
        {
            std::lock_guard<std::mutex> _l(events_guard);
            std::swap(events, es);
        }
        if (state != State::Running)
            break;
        std::lock_guard<std::mutex> _l(listeners_guard);
        for (const auto& e : es) {
            if (state != State::Running)
                break;
            auto& ps = events_id_priority_listeners[e.source];
            if (state != State::Running)
                break;
            for (auto& p : ps) {
                if (state != State::Running)
                    break;
                auto& ls = p.second;
                if (state != State::Running)
                    break;
                for (auto& l : ls) {
                    if (state != State::Running)
                        break;
                    if (l->on_event(e)) {
                        if (state != State::Running)
                            break;
                        goto event_processed;
                    }
                    if (state != State::Running)
                        break;
                }
                if (state != State::Running)
                    break;
            }
        event_processed:;
            if (state != State::Running)
                break;
        }
        if (state != State::Running)
            break;
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

void gearoenix::core::event::Engine::add_listener(Id event_id, Real priority, Listener* listener) noexcept
{
    std::lock_guard<std::mutex> _l(listeners_guard);
    events_id_priority_listeners[event_id][priority].insert(listener);
}

void gearoenix::core::event::Engine::remove_listener(Id event_id, Real priority, Listener* listener) noexcept
{
    std::lock_guard<std::mutex> _l(listeners_guard);
    events_id_priority_listeners[event_id][priority].erase(listener);
}

void gearoenix::core::event::Engine::remove_listener(Id event_id, Listener* listener) noexcept
{
    std::lock_guard<std::mutex> _l(listeners_guard);
    auto& e = events_id_priority_listeners[event_id];
    for (auto& p : e)
        p.second.erase(listener);
}

void gearoenix::core::event::Engine::remove_listener(Listener* listener) noexcept
{
    std::lock_guard<std::mutex> _l(listeners_guard);
    for (auto& e : events_id_priority_listeners) {
        auto& ps = e.second;
        for (auto& p : ps)
            p.second.erase(listener);
    }
}

void gearoenix::core::event::Engine::broadcast(const Data& event_data) noexcept
{
    if (event_data.source == Id::ButtonKeyboard) {
        const auto& ke = std::get<core::event::button::KeyboardData>(event_data.data);
        if (ke.action == button::KeyboardActionId::Press) {
            pressed_keyboard_buttons.insert(ke.key);
        } else if (ke.action == button::KeyboardActionId::Release) {
            pressed_keyboard_buttons.erase(ke.key);
        }
    }

    std::lock_guard<std::mutex> _l(events_guard);
    events.push_back(event_data);
    signaler.release();
}

void gearoenix::core::event::Engine::set_mouse_position(const math::Vec2<double>& p) noexcept
{
    mouse_movement.update(math::Vec3(p, 0.0));
}

void gearoenix::core::event::Engine::set_mouse_movement(const math::Vec2<double>& position) noexcept
{
    mouse_movement.update(math::Vec3(position, 0.0));
    Data d;
    d.source = Id::MovementMouse;
    d.data = mouse_movement;
    broadcast(d);

    for (auto& ap : pressed_mouse_buttons_state) {
        auto& p = ap.second;
        const std::chrono::duration<Real> dt = mouse_movement.current_time - p.start_time;
        if (dt.count() > CLICK_THRESHOLD || mouse_movement.delta_position.length() > MOUSE_DRAG_DISTANCE_THRESHOLD) {
            d.source = Id::GestureDrag;
            gesture::Drag drag;
            drag.start_position = math::Vec3(p.starting);
            drag.start_time = p.start_time;
            drag.previous_position = math::Vec3(p.previous);
            drag.previous_time = p.previous_time;
            drag.update(mouse_movement.current_position);
            if (ap.first == button::MouseKeyId::Left) {
                d.source = Id::GestureDrag;
                d.data = drag;
                broadcast(d);
            }
            gesture::MouseDrag mouse_drag;
            mouse_drag.base = drag;
            mouse_drag.key = ap.first;
            d.source = Id::GestureMouseDrag;
            d.data = mouse_drag;
            broadcast(d);
        }
        p.previous = position;
        p.previous_time = mouse_movement.current_time;
    }
}

void gearoenix::core::event::Engine::mouse_button(const button::MouseKeyId k, const button::MouseActionId a) noexcept
{
    button::MouseData bd;
    bd.action = a;
    bd.key = k;
    bd.position = mouse_movement.current_position.xy();

    core::event::Data e = {};
    e.source = core::event::Id::ButtonMouse;
    e.data = bd;
    broadcast(e);

    if (a == button::MouseActionId::Press) {
        auto& p = pressed_mouse_buttons_state[k];
        p = MouseButtonState();
        p.starting = mouse_movement.current_position.xy();
        p.previous = mouse_movement.current_position.xy();
    } else if (a == button::MouseActionId::Release) {
        auto pi = pressed_mouse_buttons_state.find(k);
        if (pi == pressed_mouse_buttons_state.begin()) {
            auto now = std::chrono::high_resolution_clock::now();
            auto& p = pi->second;
            const std::chrono::duration<core::Real> dt = now - p.previous_time;
            if (dt.count() < CLICK_THRESHOLD) {
                bd.action = button::MouseActionId::Click;
                e.data = bd;
                broadcast(e);
            }
            pressed_mouse_buttons_state.erase(k);
        }
    }
}

bool gearoenix::core::event::Engine::is_pressed(gearoenix::core::event::button::KeyboardKeyId k) noexcept
{
    return pressed_keyboard_buttons.find(k) != pressed_keyboard_buttons.end();
}

#include "cr-ev-engine.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../sync/cr-sync-kernel-workers.hpp"
#include "cr-ev-listener.hpp"
#include <functional>

constexpr double CLICK_THRESHOLD = 0.2f;
constexpr double CLICK_DISTANCE_THRESHOLD = 0.1f;

void gearoenix::core::event::Engine::update() noexcept
{
    std::swap(current_event_index, next_event_index);
    check_window_size_state_timeout();
    update_listeners();
}

void gearoenix::core::event::Engine::receiver() noexcept
{
    events[current_event_index].clear();
}

void gearoenix::core::event::Engine::update_listeners() noexcept
{
    GX_GUARD_LOCK(events_id_priority_listeners)
    const auto fun = [](std::map<double, std::set<Listener*>>& listeners, std::optional<double> pri, Listener* o) {
        if (pri.has_value()) {
            listeners[*pri].erase(o);
        } else {
            for (auto& ls : listeners) {
                ls.second.erase(o);
            }
        }
    };
    for (const auto& [action, id, pri, o] : events_id_priority_listeners_actions) {
        if (action == Action::Add) {
            events_id_priority_listeners[*id][*pri].insert(o);
        } else {
            if (id.has_value()) {
                fun(events_id_priority_listeners[*id], pri, o);
            } else {
                for (auto& ls : events_id_priority_listeners) {
                    fun(ls.second, pri, o);
                }
            }
        }
    }
    events_id_priority_listeners_actions.clear();
}

void gearoenix::core::event::Engine::process_events(unsigned int kernel_index) noexcept
{
    GX_START_MULTITHREADED_TASKS
    for (const auto& e : events[current_event_index]) {
        GX_DO_MULTITHREADED_TASK(process(e))
    }
}

void gearoenix::core::event::Engine::process(const Data& e) noexcept
{
    auto& ps = events_id_priority_listeners[e.get_source()];
    for (auto& p : ps) {
        auto& ls = p.second;
        for (auto& l : ls) {
            if (l->on_event(e)) {
                return;
            }
        }
    }
}

void gearoenix::core::event::Engine::update_internal_states(const Data& event_data) noexcept
{
    switch (event_data.get_source()) {
    case Id::ButtonKeyboard: {
        const auto& ke = std::get<core::event::button::KeyboardData>(event_data.get_data());
        if (ke.get_action() == button::KeyboardActionId::Press) {
            pressed_keyboard_buttons.insert(ke.get_key());
        } else if (ke.get_action() == button::KeyboardActionId::Release) {
            pressed_keyboard_buttons.erase(ke.get_key());
        }
        break;
    }
    case Id::MovementMouse: {
        const auto& d = std::get<core::event::movement::Base2D>(event_data.get_data()).get_point();
        for (auto& ap : pressed_mouse_buttons_state) {
            auto& p = ap.second;
            p.update(d.get_raw_current_position(), d.get_current_position());
            const auto dt = mouse_point.get_delta_start_time();
            if (dt > CLICK_THRESHOLD || mouse_point.get_delta_start_position().length() > CLICK_DISTANCE_THRESHOLD) {
                gesture::Drag2D drag(p);
                if (ap.first == button::MouseKeyId::Left) {
                    broadcast(Data(Id::GestureDrag2D, drag));
                }
                broadcast(Data(Id::GestureMouseDrag, gesture::MouseDrag(drag, ap.first)));
            }
        }
        break;
    }
    case Id::ButtonMouse: {
        const auto& d = std::get<core::event::button::MouseData>(event_data.get_data());
        const auto a = d.get_action();
        const auto k = d.get_key();
        if (a == button::MouseActionId::Press) {
            pressed_mouse_buttons_state.emplace(std::make_pair(
                k, Point2D(mouse_point.get_raw_current_position(), mouse_point.get_current_position())));
        } else if (a == button::MouseActionId::Release) {
            auto pi = pressed_mouse_buttons_state.find(k);
            if (pi == pressed_mouse_buttons_state.begin()) {
                auto now = std::chrono::high_resolution_clock::now();
                auto& p = pi->second;
                const std::chrono::duration<double> dt = now - p.get_current_time();
                if (dt.count() < CLICK_THRESHOLD) {
                    broadcast(Data(core::event::Id::ButtonMouse,
                        button::MouseData(
                            button::MouseActionId::Click, k,
                            mouse_point.get_current_position(),
                            mouse_point.get_raw_current_position())));
                }
                pressed_mouse_buttons_state.erase(k);
            }
        }
        break;
    }
    default:
        break;
    }
}

void gearoenix::core::event::Engine::check_window_size_state_timeout() noexcept
{
    if (window_width == previous_window_width && window_height == previous_window_height)
        return;
    if (std::chrono::high_resolution_clock::now() - previous_window_size_update < std::chrono::milliseconds(500))
        return;
    broadcast(Data(
        Id::SystemWindowSizeChange,
        system::WindowSizeChangeData {
            .previous_width = previous_window_width,
            .previous_height = previous_window_height,
            .previous_reversed_half_width = previous_window_reversed_half_width,
            .previous_reversed_half_height = previous_window_reversed_half_height,
            .previous_ratio = previous_window_ratio,
            .current_width = window_width,
            .current_height = window_height,
            .current_reversed_half_width = window_reversed_half_width,
            .current_reversed_half_height = window_reversed_half_height,
            .current_ratio = window_ratio,
            .delta_width = window_width - previous_window_width,
            .delta_height = window_height - previous_window_height,
            .delta_reversed_half_width = window_reversed_half_width - previous_window_reversed_half_width,
            .delta_reversed_half_height = window_reversed_half_height - previous_window_reversed_half_height,
            .delta_ratio = window_ratio - previous_window_ratio,
        }));
    set_previous_window_size();
}

void gearoenix::core::event::Engine::set_window_size(const int w, const int h) noexcept
{
    window_width = w;
    window_height = h;
    window_reversed_half_width = 2.0 / static_cast<double>(w);
    window_reversed_half_height = 2.0 / static_cast<double>(h);
    window_ratio = window_reversed_half_height / window_reversed_half_width;
}

void gearoenix::core::event::Engine::set_previous_window_size() noexcept
{
    previous_window_width = window_width;
    previous_window_height = window_height;
    previous_window_reversed_half_width = window_reversed_half_width;
    previous_window_reversed_half_height = window_reversed_half_height;
    previous_window_ratio = window_ratio;
}

double gearoenix::core::event::Engine::convert_raw_x(const int x) const noexcept
{
    return static_cast<double>(x) * window_reversed_half_height - window_ratio;
}

double gearoenix::core::event::Engine::convert_raw_y(const int y) const noexcept
{
    return static_cast<double>(y) * window_reversed_half_height - 1.0;
}

gearoenix::math::Vec2<double> gearoenix::core::event::Engine::convert_raw(const int x, const int y) const noexcept
{
    return math::Vec2(convert_raw_x(x), convert_raw_y(y));
}

gearoenix::core::event::Engine::Engine() noexcept
    : mouse_point({ 0, 0 }, { 0.0, 0.0 })
{
}

gearoenix::core::event::Engine::~Engine() noexcept = default;

void gearoenix::core::event::Engine::add_listener(Id event_id, double priority, Listener* listener) noexcept
{
    GX_GUARD_LOCK(events_id_priority_listeners)
    events_id_priority_listeners_actions.emplace_back(Action::Add, event_id, priority, listener);
}

void gearoenix::core::event::Engine::remove_listener(Id event_id, double priority, Listener* listener) noexcept
{
    GX_GUARD_LOCK(events_id_priority_listeners)
    events_id_priority_listeners_actions.emplace_back(Action::Remove, event_id, priority, listener);
}

void gearoenix::core::event::Engine::remove_listener(Id event_id, Listener* listener) noexcept
{
    GX_GUARD_LOCK(events_id_priority_listeners)
    events_id_priority_listeners_actions.emplace_back(Action::Remove, event_id, std::nullopt, listener);
}

void gearoenix::core::event::Engine::remove_listener(Listener* listener) noexcept
{
    GX_GUARD_LOCK(events_id_priority_listeners)
    events_id_priority_listeners_actions.emplace_back(Action::Remove, std::nullopt, std::nullopt, listener);
}

void gearoenix::core::event::Engine::broadcast(const Data& event_data) noexcept
{
    {
        GX_GUARD_LOCK(events)
        events[next_event_index].push_back(event_data);
    }
    update_internal_states(event_data);
}

void gearoenix::core::event::Engine::initialize_mouse_position(const int x, const int y) noexcept
{
    mouse_point = Point2D({ x, y }, convert_raw(x, y));
}

void gearoenix::core::event::Engine::update_mouse_position(const int x, const int y) noexcept
{
    const math::Vec2<int> rp(x, y);
    const auto cp = convert_raw(x, y);
    mouse_point.update(rp, cp);
    broadcast(Data(Id::MovementMouse, movement::Base2D(mouse_point)));
}

void gearoenix::core::event::Engine::mouse_button(const button::MouseKeyId k, const button::MouseActionId a) noexcept
{
    broadcast(Data(
        core::event::Id::ButtonMouse,
        button::MouseData(
            a, k,
            mouse_point.get_current_position(),
            mouse_point.get_raw_current_position())));
}

bool gearoenix::core::event::Engine::is_pressed(gearoenix::core::event::button::KeyboardKeyId k) const noexcept
{
    return pressed_keyboard_buttons.find(k) != pressed_keyboard_buttons.end();
}

void gearoenix::core::event::Engine::initialize_window_size(const int w, const int h) noexcept
{
    set_window_size(w, h);
    set_previous_window_size();
}

void gearoenix::core::event::Engine::update_window_size(const int w, const int h) noexcept
{
    set_window_size(w, h);
    previous_window_size_update = std::chrono::high_resolution_clock::now();
}

void gearoenix::core::event::Engine::touch_down(const touch::FingerId finger_id, const int x, const int y) noexcept
{
    for (auto& t : touch_states)
        t.second.reinitialize();
    const Point2D p(math::Vec2(x, y), convert_raw(x, y));
    const touch::State t(finger_id, p);
    touch_states.emplace(std::make_pair(finger_id, p));
    broadcast(Data(Id::Touch, touch::Data(t, touch::Action::Down)));
    click_enabled = touch_states.size() == 1;
}

void gearoenix::core::event::Engine::touch_move(const touch::FingerId finger_id, const int x, const int y) noexcept
{
    const auto search = touch_states.find(finger_id);
    if (search == touch_states.end())
        return;
    Point2D& p = search->second;
    p.update(math::Vec2(x, y), convert_raw(x, y));
    broadcast(Data(Id::Touch, touch::Data(touch::State(finger_id, p), touch::Action::Move)));
    switch (touch_states.size()) {
    case 1: {
        const auto dt = p.get_delta_start_time();
        if (dt > CLICK_THRESHOLD || p.get_delta_start_position().length() > CLICK_DISTANCE_THRESHOLD) {
            gesture::Drag2D drag(p);
            broadcast(Data(Id::GestureDrag2D, drag));
            broadcast(Data(Id::GestureTouchDrag, gesture::TouchDrag(drag, finger_id)));
        }
        break;
    }
    case 2: {
        auto iter = touch_states.begin();
        const auto& p1 = iter->second;
        const auto fi1 = iter->first;
        ++iter;
        const auto& p2 = iter->second;
        const auto fi2 = iter->first;
        const gesture::Scale scale(p1, p2);
        broadcast(Data(Id::GestureScale, scale));
        broadcast(Data(Id::GestureTouchScale, gesture::TouchScale(scale, fi1, fi2)));
        break;
    }
    default:
        break;
    }
}

void gearoenix::core::event::Engine::touch_up(const touch::FingerId finger_id, const int, const int) noexcept
{
    const auto search = touch_states.find(finger_id);
    if (search == touch_states.end())
        return;
    const Point2D p = search->second;
    touch_states.erase(search);
    for (auto& t : touch_states)
        t.second.reinitialize();
    broadcast(Data(Id::Touch, touch::Data(touch::State(finger_id, p), touch::Action::Up)));
    if (click_enabled && touch_states.empty()) {
        const auto dt = p.get_delta_start_time();
        if (dt <= CLICK_THRESHOLD || p.get_delta_start_position().length() <= CLICK_DISTANCE_THRESHOLD) {
            gesture::Click click(p);
            broadcast(Data(Id::GestureClick, click));
            broadcast(Data(Id::GestureTouchClick, gesture::TouchClick(click, finger_id)));
        }
    }
}

void gearoenix::core::event::Engine::touch_cancel(const touch::FingerId finger_id, const int, const int) noexcept
{
    const auto search = touch_states.find(finger_id);
    if (search == touch_states.end())
        return;
    const Point2D p = search->second;
    touch_states.erase(search);
    for (auto& t : touch_states)
        t.second.reinitialize();
    broadcast(Data(Id::Touch, touch::Data(touch::State(finger_id, p), touch::Action::Cancel)));
}

void gearoenix::core::event::Engine::initialize_render_engine(render::engine::Engine* render_engine) noexcept
{
    auto* const kernel_workers = render_engine->get_kernels();
    kernels_count = kernel_workers->get_threads_count();
    kernel_workers->add_step(
        [this] { update(); },
        [this](const unsigned int ki) { process_events(ki); },
        [] {},
        [this] { receiver(); });
}

gearoenix::math::Vec2<double> gearoenix::core::event::Engine::convert_raw(const math::Vec2<int>& p) const noexcept
{
    return convert_raw(p.x, p.y);
}

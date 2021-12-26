#ifndef GEAROENIX_CORE_EVENT_ENGINE_HPP
#define GEAROENIX_CORE_EVENT_ENGINE_HPP

#include "../../math/gx-math-vector-2d.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../gx-cr-types.hpp"
#include "gx-cr-ev-event.hpp"
#include "gx-cr-ev-id.hpp"
#include "gx-cr-ev-touch.hpp"
#include <chrono>
#include <map>
#include <memory>
#include <set>
#include <tuple>

#ifndef GX_DEFAULT_WINDOW_WIDTH
#define GX_DEFAULT_WINDOW_WIDTH 1
#define GX_UNDEF_DEFAULT_WINDOW_WIDTH
#endif

#ifndef GX_DEFAULT_WINDOW_HEIGHT
#define GX_DEFAULT_WINDOW_HEIGHT 1
#define GX_UNDEF_DEFAULT_WINDOW_HEIGHT
#endif

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::core::event {
struct Listener;
struct Engine {
public:
    typedef std::map<button::MouseKeyId, Point2D> MouseStateMap;
    typedef std::map<touch::FingerId, Point2D> TouchStateMap;

    GX_GET_CREF_PRV(MouseStateMap, pressed_mouse_buttons_state)
    GX_GET_CREF_PRV(TouchStateMap, touch_states)
    GX_GET_CREF_PRV(std::set<button::KeyboardKeyId>, pressed_keyboard_buttons)
    GX_GET_VAL_PRV(int, window_width, GX_DEFAULT_WINDOW_WIDTH)
    GX_GET_VAL_PRV(int, window_height, GX_DEFAULT_WINDOW_HEIGHT)
    GX_GET_VAL_PRV(double, window_ratio, static_cast<double>(GX_DEFAULT_WINDOW_WIDTH) / static_cast<double>(GX_DEFAULT_WINDOW_HEIGHT))
    GX_GET_VAL_PRV(double, window_reversed_half_width, 2.0 / static_cast<double>(GX_DEFAULT_WINDOW_WIDTH))
    GX_GET_VAL_PRV(double, window_reversed_half_height, 2.0 / static_cast<double>(GX_DEFAULT_WINDOW_HEIGHT))

private:
    GX_CREATE_GUARD(events)
    std::vector<Data> events[2];
    std::size_t current_event_index = 0, next_event_index = 1;
    std::chrono::high_resolution_clock::time_point previous_window_size_update = std::chrono::high_resolution_clock::now();
    GX_CREATE_GUARD(events_id_priority_listeners)
    std::map<Id, std::map<double, std::set<Listener*>>> events_id_priority_listeners;
    enum struct Action { Add,
        Remove };
    std::vector<std::tuple<Action, std::optional<Id>, std::optional<double>, Listener*>> events_id_priority_listeners_actions;
    Point2D mouse_point;
    bool click_enabled = true;
    int previous_window_width = 0;
    int previous_window_height = 0;
    double previous_window_reversed_half_width = 0.0;
    double previous_window_reversed_half_height = 0.0;
    double previous_window_ratio = 0.0;
    unsigned int kernels_count = 0;

    void update() noexcept;
    void receiver() noexcept;
    void update_listeners() noexcept;
    void process_events(unsigned int kernel_index) noexcept;
    void check_window_size_state_timeout() noexcept;
    void process(const Data& event_data) noexcept;
    void update_internal_states(const Data& event_data) noexcept;
    void set_window_size(int w, int h) noexcept;
    void set_previous_window_size() noexcept;

public:
    Engine() noexcept;
    ~Engine() noexcept;
    void add_listener(Id event_id, double priority, Listener* listener) noexcept;
    /// \note This is the best function to remove a listener.
    void remove_listener(Id event_id, double priority, Listener* listener) noexcept;
    void remove_listener(Id event_id, Listener* listener) noexcept;
    void remove_listener(Listener* listener) noexcept;
    void broadcast(const Data& event_data) noexcept;
    void initialize_mouse_position(int x, int y) noexcept;
    void update_mouse_position(int x, int y) noexcept;
    void mouse_button(button::MouseKeyId key, button::MouseActionId action) noexcept;
    [[nodiscard]] bool is_pressed(button::KeyboardKeyId k) const noexcept;
    void initialize_window_size(int w, int h) noexcept;
    void update_window_size(int w, int h) noexcept;
    void touch_down(touch::FingerId finger_id, int x, int y) noexcept;
    void touch_move(touch::FingerId finger_id, int x, int y) noexcept;
    void touch_up(touch::FingerId finger_id, int x, int y) noexcept;
    void touch_cancel(touch::FingerId finger_id, int x, int y) noexcept;
    void initialize_render_engine(render::engine::Engine* render_engine) noexcept;
    [[nodiscard]] double convert_raw_x(int x) const noexcept;
    [[nodiscard]] double convert_raw_y(int y) const noexcept;
    [[nodiscard]] math::Vec2<double> convert_raw(int x, int y) const noexcept;
    [[nodiscard]] math::Vec2<double> convert_raw(const math::Vec2<int>& p) const noexcept;
};
}

#ifdef GX_UNDEF_DEFAULT_WINDOW_WIDTH
#undef GX_DEFAULT_WINDOW_WIDTH
#endif

#ifdef GX_UNDEF_DEFAULT_WINDOW_HEIGHT
#undef GX_DEFAULT_WINDOW_HEIGHT
#endif

#endif
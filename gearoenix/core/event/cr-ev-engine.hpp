#ifndef GEAROENIX_CORE_EVENT_ENGINE_HPP
#define GEAROENIX_CORE_EVENT_ENGINE_HPP
#include "../../math/math-vector-2d.hpp"
#include "../cr-build-configuration.hpp"
#include "../cr-static.hpp"
#include "../cr-types.hpp"
#include "../sync/cr-sync-semaphore.hpp"
#include "cr-ev-event.hpp"
#include "cr-ev-id.hpp"
#include "cr-ev-touch.hpp"
#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

namespace gearoenix::core::event {
class Listener;
class Engine {
public:
    struct MouseButtonState {
        math::Vec2<double> starting;
        math::Vec2<double> previous;
        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
    };

    typedef std::map<button::MouseKeyId, MouseButtonState> MouseButtonStateMap;
    typedef std::map<touch::FingerId, touch::State> TouchStateMap;

    GX_GET_CREF_PRV(MouseButtonStateMap, pressed_mouse_buttons_state)
    GX_GET_CREF_PRV(TouchStateMap, touch_states)
    GX_GET_CREF_PRV(std::set<button::KeyboardKeyId>, pressed_keyboard_buttons)
#ifdef GX_FULLSCREEN
    GX_GET_VAL_PRV(std::size_t, window_width, 0)
    GX_GET_VAL_PRV(std::size_t, window_height, 0)
    GX_GET_VAL_PRV(double, window_ratio, 0.0)
    GX_GET_VAL_PRV(double, window_reversed_half_width, 0.0)
    GX_GET_VAL_PRV(double, window_reversed_half_height, 0.0)
#else
    GX_GET_VAL_PRV(std::size_t, window_width, GX_DEFAULT_WINDOW_WIDTH)
    GX_GET_VAL_PRV(std::size_t, window_height, GX_DEFAULT_WINDOW_HEIGHT)
    GX_GET_VAL_PRV(double, window_ratio, static_cast<double>(GX_DEFAULT_WINDOW_WIDTH) / static_cast<double>(GX_DEFAULT_WINDOW_HEIGHT))
    GX_GET_VAL_PRV(double, window_reversed_half_width, 2.0 / static_cast<double>(GX_DEFAULT_WINDOW_WIDTH))
    GX_GET_VAL_PRV(double, window_reversed_half_height, 2.0 / static_cast<double>(GX_DEFAULT_WINDOW_HEIGHT))
#endif
private:
    enum struct State : int {
        Running = 1,
        Terminating = 2,
        Terminated = 3,
    };

    std::atomic<State> state;
    sync::Semaphore signaler;
    std::mutex events_guard;
    std::vector<Data> events;
    std::mutex listeners_guard;
    std::map<Id, std::map<double, std::set<Listener*>>> events_id_priority_listeners;
    std::thread event_thread;

    movement::Base mouse_movement;

    std::size_t previous_window_width = 0;
    std::size_t previous_window_height = 0;
    double previous_window_reversed_half_width = 0.0;
    double previous_window_reversed_half_height = 0.0;
    double previous_window_ratio = 0.0;
    std::chrono::high_resolution_clock::time_point previous_window_size_update = std::chrono::high_resolution_clock::now();

    void loop() noexcept;

    [[nodiscard]] bool update_window_size_state(const Data& event_data) noexcept;
    void check_window_size_state_timeout() noexcept;
    void set_window_size(std::size_t w, std::size_t h) noexcept;
    void set_previous_window_size() noexcept;
    [[nodiscard]] double convert_raw_x(int x) const noexcept;
    [[nodiscard]] double convert_raw_y(int y) const noexcept;
    [[nodiscard]] math::Vec2<double> convert_raw(int x, int y) const noexcept;

public:
    Engine() noexcept;
    ~Engine() noexcept;
    void add_listener(Id event_id, double priority, Listener* listener) noexcept;
    // Best function to remove listener
    void remove_listener(Id event_id, double priority, Listener* listener) noexcept;
    void remove_listener(Id event_id, Listener* listener) noexcept;
    void remove_listener(Listener* listener) noexcept;
    void broadcast(const Data& event_data) noexcept;
    void initialize_mouse_position(int x, int y) noexcept;
    void update_mouse_position(int x, int y) noexcept;
    void mouse_button(button::MouseKeyId key, button::MouseActionId action) noexcept;
    [[nodiscard]] bool is_pressed(button::KeyboardKeyId k) const noexcept;
    void initialize_window_size(std::size_t w, std::size_t h) noexcept;
    void update_window_size(std::size_t w, std::size_t h) noexcept;
    void touch_down(touch::FingerId finger_id, int x, int y) noexcept;
    void touch_move(touch::FingerId finger_id, int x, int y) noexcept;
    void touch_up(touch::FingerId finger_id, int x, int y) noexcept;
    void touch_cancel(touch::FingerId finger_id, int x, int y) noexcept;
};
}
#endif
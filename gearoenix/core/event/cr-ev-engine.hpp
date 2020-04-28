#ifndef GEAROENIX_CORE_EVENT_ENGINE_HPP
#define GEAROENIX_CORE_EVENT_ENGINE_HPP
#include "../../math/math-vector-2d.hpp"
#include "../cr-static.hpp"
#include "../cr-types.hpp"
#include "../sync/cr-sync-semaphore.hpp"
#include "cr-ev-event.hpp"
#include "cr-ev-id.hpp"
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
    GX_GET_CREF_PRV(std::set<button::KeyboardKeyId>, pressed_keyboard_buttons)
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

    struct MouseButtonState {
        math::Vec2<double> starting;
        math::Vec2<double> previous;
        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
    };

    std::map<button::MouseKeyId, MouseButtonState> pressed_mouse_buttons_state;

    movement::Base mouse_movement;

    std::size_t window_width = 0;
    std::size_t window_height = 0;

    double window_rev_half_width = 0.0;
    double window_rev_half_height = 0.0;

    void loop() noexcept;

public:
    Engine() noexcept;
    ~Engine() noexcept;
    void add_listener(Id event_id, double priority, Listener* listener) noexcept;
    // Best function to remove listener
    void remove_listener(Id event_id, double priority, Listener* listener) noexcept;
    void remove_listener(Id event_id, Listener* listener) noexcept;
    void remove_listener(Listener* listener) noexcept;
    void broadcast(const Data& event_data) noexcept;
    void set_mouse_position(const math::Vec2<double>& position) noexcept;
    void set_mouse_movement(const math::Vec2<double>& position) noexcept;
    void mouse_button(button::MouseKeyId key, button::MouseActionId action) noexcept;
    bool is_pressed(button::KeyboardKeyId k) noexcept;
    void init_window_size(std::size_t w, std::size_t h) noexcept;
    void set_window_size(std::size_t w, std::size_t h) noexcept;
};
}
#endif
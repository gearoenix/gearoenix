#pragma once
#include "../core/event/gx-cr-ev-point.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../math/gx-math-vector-2d.hpp"
#include "gx-plt-arguments.hpp"
#include "gx-plt-build-configuration.hpp"
#include "gx-plt-key.hpp"
#include "gx-plt-runtime-configuration.hpp"
#include "gx-plt-touch.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <chrono>
#include <memory>

namespace gearoenix::audio {
struct Engine;
}

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::core::event {
struct Engine;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
struct BaseApplication final {
    friend struct Application;

    typedef boost::container::flat_map<FingerId, core::event::Point2D> TouchStateMap;

    GX_GET_CREF_PRV(Arguments, arguments);
    GX_GET_VAL_PRV(bool, running, true);
    GX_GET_CREF_PRV(math::Vec2<int>, screen_size);
    GX_GET_VAL_PRV(bool, window_resizing, false);
    GX_GET_CREF_PRV(math::Vec2<int>, previous_window_size);
    GX_GET_CREF_PRV(math::Vec2<int>, window_size);
    GX_GET_CREF_PRV(math::Vec2<int>, window_position);
    GX_GET_VAL_PRV(bool, window_is_up, false);
    GX_GET_CREF_PRV(math::Vec2<double>, mouse_position);
    /// Its value is in range of ({-window_aspect, -1.0}, {+window_aspect, +1.0})
    GX_GET_CREF_PRV(math::Vec2<double>, mouse_normalised_position);
    GX_GET_CREF_PRV(math::Vec2<double>, mouse_previous_position);
    /// Its value is in range of ({-window_aspect, -1.0}, {+window_aspect, +1.0})
    GX_GET_CREF_PRV(math::Vec2<double>, mouse_previous_normalised_position);
    GX_GET_UPTR_PRV(render::engine::Engine, render_engine);
    GX_GET_UPTR_PRV(audio::Engine, audio_engine);
    GX_GET_UPTR_PRV(core::event::Engine, event_engine);
    GX_GET_UPTR_PRV(core::Application, core_application);
    GX_GET_CREF_PRV(TouchStateMap, touch_states);
    GX_GET_CREF_PRV(boost::container::flat_set<platform::key::Id>, pressed_keyboard_buttons);
    GX_GET_VAL_PRV(std::chrono::high_resolution_clock::time_point, last_time_window_resized, std::chrono::high_resolution_clock::now());

    void initialise_imgui();

public:
    explicit BaseApplication(GX_MAIN_ENTRY_ARGS_DEF);
    ~BaseApplication();
    void initialize_window_position(int x, int y);
    void initialize_window_size(int w, int h);
    void update_window_size(int w, int h);
    void update_window();
    void initialize_mouse_position(double x, double y);
    void update_mouse_position(double x, double y);
    void mouse_key(key::Id, key::Action);
    void mouse_wheel(double v);
    void keyboard_key(key::Id, key::Action);
    void character_input(char16_t ch);
    void touch_down(FingerId finger_id, double x, double y);
    void touch_move(FingerId finger_id, double x, double y);
    void touch_up(FingerId finger_id);
    void touch_cancel(FingerId finger_id);
    void initialize_engine(Application&);
    void initialize_core_application(Application&, core::Application*);
    void going_to_be_closed();
    void terminate();
    void update();

private:
    [[nodiscard]] double normalise_x(double x) const;
    [[nodiscard]] double normalise_y(double y) const;
    [[nodiscard]] math::Vec2<double> normalise_position(double x, double y) const;
};
}

#if defined(GX_PLATFORM_INTERFACE_ANDROID)
#include "android/gx-plt-and-application.hpp"
#elif defined(GX_PLATFORM_INTERFACE_WIN32)
#include "win32/gx-plt-win-application.hpp"
#elif defined(GX_PLATFORM_INTERFACE_X11)
#include "x11/gx-plt-x11-application.hpp"
#elif defined(GX_PLATFORM_MACOS)
#include "macos/gx-plt-mac-application.hpp"
#elif defined(GX_PLATFORM_INTERFACE_SDL2)
#include "sdl2/gx-plt-sdl2-application.hpp"
#else
#error "Unspecified platform application interface."
#endif
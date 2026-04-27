#pragma once
#include "../core/event/gx-cr-ev-point.hpp"
#include "../core/gx-cr-singleton.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../math/gx-math-vector-2d.hpp"
#include "gx-plt-build-configuration.hpp"
#include "gx-plt-key.hpp"
#include "gx-plt-touch.hpp"

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include <chrono>
#include <functional>
#include <memory>

namespace gearoenix::audio {
struct Engine;
}

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::core::event {
struct Engine;
}

namespace gearoenix::net {
struct Manager;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
struct BaseApplication final : core::Singleton<BaseApplication> {
    friend struct Application;

    typedef boost::container::flat_map<FingerId, core::event::Point2D> TouchStateMap;

    GX_GET_VAL_PRV(bool, running, true);
    GX_GET_CREF_PRV(math::Vec2<int>, screen_size);
    GX_GET_VAL_PRV(bool, window_resizing, false);
    GX_GET_CREF_PRV(math::Vec2<int>, previous_window_size);
    GX_GET_CREF_PRV(math::Vec2<int>, window_size);
    GX_GET_CREF_PRV(math::Vec2<int>, window_position);
    GX_GET_VAL_PRV(bool, window_is_up, false);
    GX_GETSET_VAL_PRV(bool, window_is_going_to_be_closed, false);
    GX_GET_CREF_PRV(math::Vec2<core::fp_t>, mouse_position);
    /// Its value is in the range of ({-window_aspect, -1.0}, {+window_aspect, +1.0})
    GX_GET_CREF_PRV(math::Vec2<core::fp_t>, mouse_normalised_position);
    GX_GET_CREF_PRV(math::Vec2<core::fp_t>, mouse_previous_position);
    /// Its value is in the range of ({-window_aspect, -1.0}, {+window_aspect, +1.0})
    GX_GET_CREF_PRV(math::Vec2<core::fp_t>, mouse_previous_normalised_position);
    GX_GET_UPTR_PRV(render::engine::Engine, render_engine);
    GX_GET_UPTR_PRV(audio::Engine, audio_engine);
    GX_GET_UPTR_PRV(core::event::Engine, event_engine);
    GX_GET_UPTR_PRV(core::Application, core_application);
    GX_GET_UPTR_PRV(net::Manager, net_manager);
    GX_GET_CREF_PRV(TouchStateMap, touch_states);
    GX_GETSET_CREF_PRV(std::function<bool()>, should_window_be_closed);
    GX_GET_CREF_PRV(boost::container::flat_set<platform::key::Id>, pressed_keyboard_buttons);
    GX_GET_VAL_PRV(std::chrono::high_resolution_clock::time_point, last_time_window_resized, std::chrono::high_resolution_clock::now());

    void initialise_imgui();

public:
    BaseApplication();
    ~BaseApplication() override;
    void initialize_window_position(int x, int y);
    void initialize_window_size(int w, int h);
    void update_window_size(int w, int h);
    void update_window();
    /// Called when the OS reports the window's HDR state changed (display moved between
    /// HDR/SDR monitors, system HDR toggle). Re-queries the swapchain peak / OETF and
    /// notifies cameras tracking the default LUT to swap to the matching AgX variant.
    void hdr_state_changed();
    void initialize_mouse_position(core::fp_t x, core::fp_t y);
    void update_mouse_position(core::fp_t x, core::fp_t y);
    void mouse_key(key::Id, key::Action);
    void mouse_wheel(core::fp_t v);
    void keyboard_key(key::Id, key::Action);
    void character_input(char16_t ch);
    void touch_down(FingerId finger_id, core::fp_t x, core::fp_t y);
    void touch_move(FingerId finger_id, core::fp_t x, core::fp_t y);
    void touch_up(FingerId finger_id);
    void touch_cancel(FingerId finger_id);
    void initialize_engine();
    void initialize_core_application(core::Application*);
    void close();
    void terminate();
    void update();

    /// Load default fond data in memory
    ///
    /// This function loads the default font if presented in the `assets` folder
    ///
    /// @return If the default font file isn't in the `assets` folder,
    ///         it will return null otherwise a loaded font data.
    ///         The second return value is the size of the loaded data.
    [[nodiscard]] static std::pair<void*, int> get_default_font_data();

private:
    [[nodiscard]] core::fp_t normalise_x(core::fp_t x) const;
    [[nodiscard]] core::fp_t normalise_y(core::fp_t y) const;
    [[nodiscard]] math::Vec2<core::fp_t> normalise_position(core::fp_t x, core::fp_t y) const;
};
}

#if GX_PLATFORM_INTERFACE_ANDROID
#include "android/gx-plt-and-application.hpp"
#elif GX_PLATFORM_INTERFACE_WIN32
#include "win32/gx-plt-win-application.hpp"
#elif GX_PLATFORM_INTERFACE_X11
#include "x11/gx-plt-x11-application.hpp"
#elif GX_PLATFORM_INTERFACE_APPKIT
#include "macos/gx-plt-mac-application.hpp"
#elif GX_PLATFORM_INTERFACE_SDL
#include "sdl/gx-plt-sdl-application.hpp"
#else
#error "Unspecified platform application interface."
#endif
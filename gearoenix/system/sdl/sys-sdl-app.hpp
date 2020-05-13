#ifndef GEAROENIX_SYSTEM_SDL_APP_HPP
#define GEAROENIX_SYSTEM_SDL_APP_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_SDL
#include "../../core/cr-static.hpp"
#include "../../render/engine/rnd-eng-type.hpp"
#include "../sys-configuration.hpp"
#define NO_SDL_GLEXT
#include <SDL.h>
#include <memory>
#ifdef GX_IN_WEB
#include <emscripten.h>
#endif
#include <optional>

namespace gearoenix::core::asset {
class Manager;
}

namespace gearoenix::core::event {
class Engine;
}

namespace gearoenix::core {
class Application;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::system {
class Args;
class Application {
    GX_GET_REF_PRV(Configuration, configuration)
    GX_GET_UPTR_PRV(render::engine::Engine, render_engine)
    GX_GET_UPTR_PRV(core::Application, core_application)
    GX_GET_UPTR_PRV(core::asset::Manager, asset_manager)
    GX_GET_UPTR_PRV(core::event::Engine, event_engine)
    GX_GET_UPTR_PRV(Args, arguments)
private:
#ifdef GX_IN_WEB
    static Application* app;
#endif
    const static double ROTATION_EPSILON;
    const static double ZOOM_EPSILON;

    SDL_Window* window = nullptr;

#ifdef GX_USE_OPENGL
    SDL_GLContext gl_context = nullptr;
#endif

    bool running = true;
    std::optional<bool> gooing_to_show_keyboard = std::nullopt;
    render::engine::Type supported_engine = render::engine::Type::NONE;

    void create_window() noexcept;
    static int SDLCALL event_receiver(void* user_data, SDL_Event* event) noexcept;
    int on_event(SDL_Event* event) noexcept;

public:
    Application(int argc, const char* const* argv) noexcept;
    ~Application() noexcept;
    void execute(std::unique_ptr<core::Application> app) noexcept;

#ifdef GX_IN_WEB
    static void loop() noexcept;
    void main_loop() noexcept;
#endif
    [[nodiscard]] const char* get_clipboard() const noexcept;
    /// It is going to break the main loop an then smooth exit will happen
    /// Core application termination will be called, and then termination of other modules
    void quit() noexcept;
    void set_soft_keyboard_visibility(bool show) noexcept;
};
}
#endif
#endif

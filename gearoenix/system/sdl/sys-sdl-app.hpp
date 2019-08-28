#ifndef GEAROENIX_SYSTEM_SDL_APP_HPP
#define GEAROENIX_SYSTEM_SDL_APP_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_SDL
#include "../../core/cr-types.hpp"
#include "../../render/engine/rnd-eng-type.hpp"
#include "../sys-configuration.hpp"
#define NO_SDL_GLEXT
#include <SDL.h>
#include <memory>
#ifdef GX_IN_WEB
#include <emscripten.h>
#endif
namespace gearoenix {
namespace core {
    namespace asset {
        class Manager;
    }
    namespace event {
        class Engine;
    }
    class Application;
}
namespace render::engine {
    class Engine;
}
namespace system {
    class Application {
    private:
#ifdef GX_IN_WEB
        static Application* app;
#endif
        const static core::Real ROTATION_EPSILON;
        const static core::Real ZOOM_EPSILON;

        Configuration config;
        render::engine::Engine* render_engine = nullptr;
        core::Application* core_app = nullptr;
        core::asset::Manager* astmgr = nullptr;
        core::event::Engine* event_engine = nullptr;
        /// TODO: All the surface functionality most move to new class
        unsigned int win_width = 0;
        unsigned int win_height = 0;

        core::Real screen_ratio = 0.0f;
        core::Real half_height_inversed = 0.0f;
        core::Real pre_x = 0.0f;
        core::Real pre_y = 0.0f;

        SDL_Window* window = nullptr;

#ifdef GX_USE_OPENGL
        SDL_GLContext gl_context = nullptr;
#endif

        bool running = true;
        render::engine::Type supported_engine = render::engine::Type::None;

        void create_window() noexcept;
        static int SDLCALL event_receiver(void* user_data, SDL_Event* event) noexcept;

        Application() noexcept = default;

    public:
        static const std::shared_ptr<Application> construct() noexcept;
        ~Application() noexcept;
        void execute(core::Application* app) noexcept;

#ifdef GX_IN_WEB
        static void loop() noexcept;
        void main_loop() noexcept;
#endif

        const core::Application* get_core_app() const noexcept;
        core::Application* get_core_app() noexcept;
        const render::engine::Engine* get_render_engine() const noexcept;
        render::engine::Engine* get_render_engine() noexcept;
        const core::asset::Manager* get_asset_manager() const noexcept;
        core::asset::Manager* get_asset_manager() noexcept;
        const core::event::Engine* get_event_engine() const noexcept;
        core::event::Engine* get_event_engine() noexcept;
        const Configuration& get_configuration() const noexcept;
        Configuration& get_configuration() noexcept;
        core::Real get_window_ratio() const noexcept;
        unsigned int get_width() const noexcept;
        unsigned int get_height() const noexcept;
        core::Real convert_x_to_ratio(int x) const noexcept;
        core::Real convert_y_to_ratio(int y) const noexcept;
    };
}
}
#endif
#endif

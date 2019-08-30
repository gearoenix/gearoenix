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
		GX_GET_REF_PRV(Configuration, configuration)
		GX_GET_PTR_PRV(render::engine::Engine, render_engine)
		GX_GET_PTR_PRV(core::Application, core_app)
		GX_GET_PTR_PRV(core::asset::Manager, asset_manager)
		GX_GET_PTR_PRV(core::event::Engine, event_engine)
		GX_GET_VAL_PRV(unsigned int, window_width, 0)
		GX_GET_VAL_PRV(unsigned int, window_height, 0)
		GX_GET_VAL_PRV(core::Real, window_ratio, 1.0F)
    private:
#ifdef GX_IN_WEB
        static Application* app;
#endif
        const static core::Real ROTATION_EPSILON;
        const static core::Real ZOOM_EPSILON;

        core::Real half_height_inversed = 0.0f;
        core::Real pre_x = 0.0f;
        core::Real pre_y = 0.0f;

        SDL_Window* window = nullptr;

#ifdef GX_USE_OPENGL
        SDL_GLContext gl_context = nullptr;
#endif

        bool running = true;
        render::engine::Type supported_engine = render::engine::Type::NONE;

        void create_window() noexcept;
        static int SDLCALL event_receiver(void* user_data, SDL_Event* event) noexcept;

        Application() noexcept = default;

    public:
        static Application * construct() noexcept;
        ~Application() noexcept;
        void execute(core::Application* app) noexcept;

#ifdef GX_IN_WEB
        static void loop() noexcept;
        void main_loop() noexcept;
#endif
        core::Real convert_x_to_ratio(int x) const noexcept;
        core::Real convert_y_to_ratio(int y) const noexcept;
    };
}
}
#endif
#endif

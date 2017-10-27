#ifndef GEAROENIX_SYSTEM_SDL_APP_HPP
#define GEAROENIX_SYSTEM_SDL_APP_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_SDL
#include "../../core/cr-types.hpp"
#ifdef IN_IOS
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
namespace gearoenix {
namespace core {
    namespace asset {
        class Manager;
    }
    class Application;
}
namespace render {
    class Engine;
}
namespace system {
    class Application {
    private:
        const static core::Real rotate_epsilon;
        const static core::Real zoom_epsilon;
        core::Application* core_app = nullptr;
        render::Engine* render_engine = nullptr;
        core::asset::Manager* astmgr = nullptr;
        SDL_Window* window = nullptr;
        SDL_GLContext gl_context;
        bool running = true;
        static int SDLCALL event_receiver(void* user_data, SDL_Event* event);

    public:
        Application();
        ~Application();
        void execute(core::Application* app);
        const core::Application* get_core_app() const;
        core::Application* get_core_app();
        const render::Engine* get_render_engine() const;
        render::Engine* get_render_engine();
        const core::asset::Manager* get_asset_manager() const;
        core::asset::Manager* get_asset_manager();
        core::Real get_window_ratio() const;
        unsigned int get_width() const;
        unsigned int get_height() const;
    };
}
}
#endif
#endif

#ifndef GEAROENIX_SYSTEM_SDL_APP_HPP
#define GEAROENIX_SYSTEM_SDL_APP_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_SDL
#include "../../core/cr-types.hpp"
#include "../../render/engine/rnd-eng-type.hpp"
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
    class Application;
}
namespace render {
	namespace engine {
		class Engine;
	}
}
namespace system {
    class Application {
    private:

#ifdef GX_IN_WEB
        static Application* app;
#endif
        const static core::Real ROTATION_EPSILON;
        const static core::Real ZOOM_EPSILON;

        std::shared_ptr<render::engine::Engine> render_engine = nullptr;
        std::shared_ptr<core::Application> core_app = nullptr;
        std::shared_ptr<core::asset::Manager> astmgr = nullptr;

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
        render::engine::Type::Id supported_engine = render::engine::Type::UNKNOWN;

        void create_window() noexcept;
        void create_context() noexcept;
        static int SDLCALL event_receiver(void* user_data, SDL_Event* event) noexcept;

        Application() noexcept;

    public:

		static const std::shared_ptr<Application> construct() noexcept;
        ~Application();
        void execute(const std::shared_ptr<core::Application> &app) noexcept;

#ifdef GX_IN_WEB
        static void loop() noexcept;
        void main_loop() noexcept;
#endif

        const std::shared_ptr<core::Application> &get_core_app() const noexcept;
		std::shared_ptr<core::Application> &get_core_app() noexcept;
        const std::shared_ptr<render::engine::Engine> &get_render_engine() const noexcept;
		std::shared_ptr<render::engine::Engine> &get_render_engine() noexcept;
        const std::shared_ptr<core::asset::Manager> &get_asset_manager() const noexcept;
		std::shared_ptr<core::asset::Manager> &get_asset_manager() noexcept;
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

#ifndef GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#define GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include "../../core/cr-types.hpp"
#include "../sys-configuration.hpp"
#include "../../render/engine/rnd-eng-type.hpp"

struct android_app;
struct AInputEvent;

namespace gearoenix {
namespace render {
    namespace engine {
        class Engine;
    }
}
namespace core {
    namespace asset {
        class Manager;
    }
    namespace event {
        class Engine;
    }
    class Application;
}
namespace system {
    class File;
    class GlContext;
    class Application {
    private:
        bool running = true;
        Configuration config;
        android_app* and_app = nullptr;
        render::engine::Engine* render_engine = nullptr;
        core::Application* core_app = nullptr;
        core::asset::Manager* astmgr = nullptr;
        core::event::Engine* event_engine = nullptr;
#ifdef GX_USE_OPENGL
        GlContext *gl_context = nullptr;
#endif
        unsigned int win_width = 0;
        unsigned int win_height = 0;
        core::Real screen_ratio = 1.0f;
        core::Real half_height_inverted = 1.0f;
        core::Real x = 0.0f;
        core::Real y = 0.0f;
        core::Real w = 0.0f;
        render::engine::Type supported_engine = render::engine::Type::None;
        void handle(android_app* app, int32_t cmd) noexcept;
        int32_t handle(android_app* app, AInputEvent* e) noexcept;
        static void handle_cmd(android_app* app, int32_t cmd) noexcept;
        static int32_t handle_input(android_app* app, AInputEvent* e) noexcept;

    public:
        Application(struct android_app* and_app) noexcept;
        ~Application() noexcept;
        void execute(core::Application* core_app) noexcept;
        android_app* get_android_app() const noexcept;
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
#endif // GX_IN_ANDROID && !GX_USE_SDL
#endif // GEAROENIX_SYSTEM_ANDROID_HPP

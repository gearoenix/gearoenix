#ifndef GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#define GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include "../../core/cr-types.hpp"
#include "../sys-configuration.hpp"
#include "../../core/cr-static.hpp"
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
        GX_GET_REF_PRV(Configuration, configuration)
        GX_GET_PTR_PRV(android_app, android_application)
        GX_GET_PTR_PRV(render::engine::Engine, render_engine)
        GX_GET_PTR_PRV(core::Application, core_app)
        GX_GET_PTR_PRV(core::asset::Manager, asset_manager)
        GX_GET_PTR_PRV(core::event::Engine, event_engine)
        GX_GET_VAL_PRV(unsigned int, window_width, 0)
        GX_GET_VAL_PRV(unsigned int, window_height, 0)
        GX_GET_VAL_PRV(core::Real, window_ratio, 1.0F)
    private:
        bool running = true;
#ifdef GX_USE_OPENGL
        GlContext *gl_context = nullptr;
#endif
        core::Real half_height_inverted = 1.0f;
        core::Real x = 0.0f;
        core::Real y = 0.0f;
        core::Real w = 0.0f;
        render::engine::Type supported_engine = render::engine::Type::NONE;
        void handle(android_app* app, int32_t cmd) noexcept;
        int32_t handle(android_app* app, AInputEvent* e) noexcept;
        static void handle_cmd(android_app* app, int32_t cmd) noexcept;
        static int32_t handle_input(android_app* app, AInputEvent* e) noexcept;
        void compute_screen_ratios() noexcept;

    public:
        Application(struct android_app* and_app) noexcept;
        ~Application() noexcept;
        void execute(core::Application* core_app) noexcept;
        core::Real convert_x_to_ratio(int x) const noexcept;
        core::Real convert_y_to_ratio(int y) const noexcept;
    };
}
}
#endif // GX_IN_ANDROID && !GX_USE_SDL
#endif // GEAROENIX_SYSTEM_ANDROID_HPP

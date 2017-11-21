#ifndef GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#define GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_ANDROID
#include "../../core/cr-types.hpp"
#include <EGL/egl.h>
#include <GLES/gl.h>
struct android_app;
namespace gearoenix {
namespace render {
    class Engine;
}
namespace core {
    namespace asset {
        class Manager;
    }
    class Application;
}
namespace system {
    class File;
    class Application {
    private:
        android_app* and_app = nullptr;
        render::Engine* render_engine = nullptr;
        core::Application* core_app = nullptr;
        core::asset::Manager* astmgr = nullptr;
        unsigned int win_width, win_height;
        EGLSurface surface;
        EGLContext context;
        EGLDisplay display;
        void handle(std::int32_t cmd);
        void init();
        static void handle_cmd(android_app* app, int32_t cmd);

    public:
        Application(struct android_app* and_app);
        ~Application();
        void execute(core::Application* core_app);
        android_app* get_android_app() const;
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
#endif // IN_ANDROID
#endif // GEAROENIX_SYSTEM_ANDROID_HPP

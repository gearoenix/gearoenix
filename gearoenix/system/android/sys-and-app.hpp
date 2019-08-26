#ifndef GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#define GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include "../../core/cr-types.hpp"

struct android_app;
struct AInputEvent;

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
        bool active = false;
        android_app* and_app = nullptr;
        render::Engine* render_engine = nullptr;
        core::Application* core_app = nullptr;
        core::asset::Manager* astmgr = nullptr;
        unsigned int win_width, win_height;
        core::Real screen_ratio, half_height_inversed;
        core::Real x = 0.0f, y = 0.0f, w = 0.0f;
        void handle(android_app* app, int32_t cmd);
        int32_t handle(android_app* app, AInputEvent* e);
        static void handle_cmd(android_app* app, int32_t cmd);
        static int32_t handle_input(android_app* app, AInputEvent* e);
        core::Real convert_pixel_x_to_normalized(int x);
        core::Real convert_pixel_y_to_normalized(int y);

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

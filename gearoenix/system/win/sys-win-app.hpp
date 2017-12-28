#ifndef GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP
#define GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_WINAPI
#include "../../core/cr-types.hpp"
#include <Windows.h>
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
        int screen_width, screen_height;
        render::Engine* render_engine = nullptr;
        core::asset::Manager* astmgr = nullptr;
        core::Application* core_app = nullptr;
        HINSTANCE instance;
        HWND window;
        bool running = true;
        bool window_is_up = false;
        UINT mouse_prepos_x = 0, mouse_prepos_y = 0;
        static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
        LRESULT CALLBACK handler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
        core::Real supported_engine;

    public:
        Application();
        ~Application();
        void execute(core::Application* core_app);
        render::Engine* get_render_engine();
        const render::Engine* get_render_engine() const;
        core::asset::Manager* get_asset_manager();
        const core::asset::Manager* get_asset_manager() const;
        core::Real get_window_ratio() const;
        unsigned int get_width() const;
        unsigned int get_height() const;
        HWND get_window();
    };
}
}
#endif // USE_WINAPI
#endif // GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP

#ifndef GEAROENIX_SYSTEM_WINDOWS_WIN_APP_HPP
#define GEAROENIX_SYSTEM_WINDOWS_WIN_APP_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_WINDOWS
#include "../../render/render-engine.hpp"
#include <Windows.h>
#include <memory>
namespace gearoenix {
namespace core {
    class Application;
}
namespace system {
    class Application {
    private:
        bool running = true;
        HINSTANCE instance;
        WNDCLASSEX wnd_class;
        HWND window;
        std::shared_ptr<core::Application> core_app;
        std::shared_ptr<render::Engine> render_engine;
        static LRESULT CALLBACK process_callback(HWND hWnd, UINT uMsg, WPARAM wParam,
            LPARAM lParam);
        LRESULT handle_message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    public:
        Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
            int nCmdShow);
        ~Application();
        int execute();
        const HINSTANCE& get_hinstance() const;
        const HWND& get_hwnd() const;
        const std::shared_ptr<core::Application>& get_core_app() const;
        const std::shared_ptr<render::Engine>& get_render_engine() const;
    };
}
}
#endif
#endif

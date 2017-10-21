#ifndef GEAROENIX_SYSTEM_LINUX_APPLICATION_HPP
#define GEAROENIX_SYSTEM_LINUX_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_LINUX
#include "../../core/cr-types.hpp"
#include <xcb/xcb.h>
#ifdef USE_OPENGL
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#endif
namespace gearoenix {
namespace core {
    class Application;
    namespace asset {
        class Manager;
    }
}
namespace render {
    class Engine;
}
namespace system {
    class File;
    class Application {
    private:
        bool quit = false;
        core::Real winratio = 1.7;
        xcb_connection_t* connection;
        xcb_screen_t* screen;
        xcb_window_t window;
#ifdef USE_OPENGL
        Display* display;
        GLXDrawable drawable;
        GLXWindow glxwindow;
#endif
        xcb_intern_atom_reply_t* atom_wm_delete_window;
        core::asset::Manager* assetmgr;
        core::Application* core_app;
        render::Engine* render_engine;
        void handle(const xcb_generic_event_t* event);

    public:
        Application();
        ~Application();
        void execute(core::Application* ca);
        xcb_connection_t* get_connection() const;
        xcb_window_t get_window() const;
        core::asset::Manager* get_asset_manager();
        const core::Application* get_core_app() const;
        const render::Engine* get_render_engine() const;
        render::Engine* get_render_engine();
        core::Real get_window_ratio() const;
    };
}
}
#endif
#endif

#ifndef GEAROENIX_SYSTEM_LINUX_APPLICATION_HPP
#define GEAROENIX_SYSTEM_LINUX_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_LINUX
#include "../../core/cr-types.hpp"
#include <xcb/xcb.h>
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
        xcb_connection_t* connection;
        xcb_screen_t* screen;
        xcb_window_t window;
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
        core::Real get_screen_ratio() const;
    };
}
}
#endif
#endif

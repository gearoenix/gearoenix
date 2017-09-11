#ifndef GEAROENIX_SYSTEM_LINUX_APPLICATION_HPP
#define GEAROENIX_SYSTEM_LINUX_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_LINUX
#include <memory>
#include <xcb/xcb.h>
namespace gearoenix {
namespace core {
    class Application;
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
        std::shared_ptr<File> asset;
        std::shared_ptr<core::Application> core_app;
        std::shared_ptr<render::Engine> render_engine;
        void handle(const xcb_generic_event_t* event);

    public:
        Application();
        ~Application();
        void execute();
        xcb_connection_t* get_connection() const;
        xcb_window_t get_window() const;
        const std::shared_ptr<File> get_asset() const;
        const std::shared_ptr<core::Application>& get_core_app() const;
        const std::shared_ptr<render::Engine>& get_render_engine() const;
    };
}
}
#endif
#endif

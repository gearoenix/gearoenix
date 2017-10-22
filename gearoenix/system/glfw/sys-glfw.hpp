#ifndef GEAROENIX_SYSTEM_GLFW_APP_HPP
#define GEAROENIX_SYSTEM_GLFW_APP_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_GLFW
#include "../../core/cr-types.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
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
        core::Application* core_app = nullptr;
        render::Engine* render_engine = nullptr;
        core::asset::Manager* astmgr = nullptr;
        GLFWwindow* window = nullptr;
        unsigned int win_width, win_height;
        bool running = true;
        static std::map<GLFWwindow*, Application*> event_mapper;
        static void on_error_event(int error_number, const char* error_description);
        static void on_cursor_move_event(GLFWwindow* window, double x, double y);
        static void on_scroll(GLFWwindow* window, double x, double y);
        static void on_mouse_key_event(GLFWwindow* window, int button, int action, int mods);
        static void on_key_event(GLFWwindow* window, int key, int scan_code, int action, int mods);
        static void on_change_size_event(GLFWwindow* window, int width, int height);

    public:
        Application();
        ~Application();
        void execute(core::Application* app);
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
#endif
#endif

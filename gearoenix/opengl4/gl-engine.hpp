#ifndef GEAROENIX_OPENGL_ENGINE_HPP
#define GEAROENIX_OPENGL_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL
#include "../render/rnd-engine.hpp"
namespace gearoenix {
namespace opengl4 {
    class Engine : public render::Engine {
    private:
    public:
        Engine(system::Application* sys_app);
        ~Engine();
        void window_changed();
        void update();
        void terminate();
        static bool is_supported();
    };
}
}
#endif
#endif

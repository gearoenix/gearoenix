#ifndef GEAROENIX_GLES2_ENGINE_HPP
#define GEAROENIX_GLES2_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../render/rnd-engine.hpp"
namespace gearoenix {
namespace gles2 {
    class Engine : public render::Engine {
    private:
    public:
        Engine(system::Application* sys_app);
        ~Engine();
        void window_changed();
        void update();
        void terminate();
    };
}
}
#endif
#endif

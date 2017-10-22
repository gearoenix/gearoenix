#ifndef GEAROENIX_GLES2_ENGINE_HPP
#define GEAROENIX_GLES2_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../render/rnd-engine.hpp"
#include <SDL2/SDL_opengles2.h>
namespace gearoenix {
namespace gles2 {
    class Engine : public render::Engine {
    private:
        GLuint shadow_map_texture;
        GLuint shadow_map_framebuffer;

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

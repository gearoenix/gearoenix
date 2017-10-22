#ifndef GEAROENIX_GLES2_ENGINE_HPP
#define GEAROENIX_GLES2_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../math/math-matrix.hpp"
#include "../render/rnd-engine.hpp"
#include <SDL2/SDL_opengles2.h>
#include <memory>
namespace gearoenix {
namespace render {
    namespace camera {
        class Camera;
    }
}
namespace gles2 {
    namespace shader {
        class DirectionalColoredSpeculatedNocubeNoshadowOpaque;
    }
    class Engine : public render::Engine {
    private:
        GLuint shadow_map_texture;
        GLuint shadow_map_framebuffer;

        GLuint vbo, ibo;
        shader::DirectionalColoredSpeculatedNocubeNoshadowOpaque* shd;
        std::shared_ptr<render::camera::Camera> cam;
        math::Mat4x4 vp;

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

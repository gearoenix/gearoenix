#ifndef GEAROENIX_GLES2_ENGINE_HPP
#define GEAROENIX_GLES2_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
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
        class DirectionalTexturedSpeculatedNocubeNoshadowOpaque;
    }
    namespace texture {
        class Texture2D;
    }
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
        render::texture::Texture2D* create_texture_2d(system::File* file, std::shared_ptr<core::EndCaller> c);
        render::shader::Shader* create_shader(core::Id sid, system::File* file, std::shared_ptr<core::EndCaller> c);
    };
}
}
#endif
#endif

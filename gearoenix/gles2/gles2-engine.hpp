#ifndef GEAROENIX_GLES2_ENGINE_HPP
#define GEAROENIX_GLES2_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../render/rnd-engine.hpp"
#include "gles2.hpp"
#include <memory>
namespace gearoenix {
namespace gles2 {
    namespace texture {
        class Texture2D;
    }
    class Engine : public render::Engine {
    private:
        GLuint shadow_map_color = 0;
        GLuint shadow_map_depth = 0;
        GLuint shadow_map_framebuffer = 0;
        GLuint shadow_map_aspect = 1024;
        texture::Texture2D* shadow_map_texture = nullptr;
        GLuint render_depth = 0;
        GLuint render_framebuffer = 0;
        GLfloat win_width;
        GLfloat win_height;

    public:
        Engine(system::Application* sys_app);
        ~Engine();
        void window_changed();
        void update();
        void terminate();
        render::texture::Texture2D* create_texture_2d(system::File* file, std::shared_ptr<core::EndCaller> c);
        render::texture::Cube* create_texture_cube(system::File* file, std::shared_ptr<core::EndCaller> c);
        render::buffer::Mesh* create_mesh(unsigned int vec, system::File* file, std::shared_ptr<core::EndCaller> c);
        render::buffer::Uniform* create_uniform(unsigned int s, std::shared_ptr<core::EndCaller> c);
        render::shader::Shader* create_shader(core::Id sid, system::File* file, std::shared_ptr<core::EndCaller> c);
        render::shader::Resources* create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, std::shared_ptr<core::EndCaller> c);
        render::pipeline::Pipeline* create_pipeline(core::Id sid, std::shared_ptr<core::EndCaller> c);
    };
}
}
#endif
#endif

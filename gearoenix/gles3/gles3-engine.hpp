#ifndef GEAROENIX_GLES3_ENGINE_HPP
#define GEAROENIX_GLES3_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../render/rnd-engine.hpp"
#include "gles3.hpp"
#include <memory>
#ifdef gles3_PROFILING
#include <chrono>
#endif
namespace gearoenix {
namespace gles3 {
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
#ifdef gles3_PROFILING
        unsigned int prof_frames_count = 0;
        std::chrono::high_resolution_clock::time_point prof_last_time_draw;
        double prof_frames_time = 0.0;
#endif
    public:
        Engine(system::Application* sys_app);
        ~Engine();
        void window_changed();
        void update();
        void terminate();
        render::texture::Texture2D* create_texture_2d(system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::texture::Cube* create_texture_cube(system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::buffer::Mesh* create_mesh(unsigned int vec, system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::buffer::Uniform* create_uniform(unsigned int s, core::EndCaller<core::EndCallerIgnore> c);
        render::shader::Shader* create_shader(core::Id sid, system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::shader::Resources* create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, core::EndCaller<core::EndCallerIgnore> c);
        render::pipeline::Pipeline* create_pipeline(core::Id sid, core::EndCaller<core::EndCallerIgnore> c);
    };
}
}
#endif
#endif

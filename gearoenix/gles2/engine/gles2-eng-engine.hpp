#ifndef GEAROENIX_GLES2_ENGINE_HPP
#define GEAROENIX_GLES2_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2

#ifdef GX_PROFILING_MODE
#define GX_GLES2_ENGINE_PROFILING
#endif

#include "../../render/engine/rnd-eng-engine.hpp"
//#include "gles2.hpp"

#include <memory>
#ifdef GX_GLES2_ENGINE_PROFILING
#include <chrono>
#endif

namespace gearoenix {
namespace gles2 {
    namespace texture {
        class Texture2D;
    }
    namespace engine {
        class Engine : public render::engine::Engine {
        private:
            //        GLuint shadow_map_color = 0;
            //        GLuint shadow_map_depth = 0;
            //        GLuint shadow_map_framebuffer = 0;
            //        GLuint shadow_map_aspect = 1024;
            //        texture::Texture2D* shadow_map_texture = nullptr;
            //        GLuint render_depth = 0;
            //        GLuint render_framebuffer = 0;
            //        GLfloat win_width;
            //        GLfloat win_height;
            void initialize();
#ifdef GX_GLES2_ENGINE_PROFILING
			/// todo: create a class for profilling
            unsigned int prof_frames_count = 0;
            std::chrono::high_resolution_clock::time_point prof_last_time_draw;
            double prof_frames_time = 0.0;
#endif
            Engine(const std::shared_ptr<system::Application>& sys_app);
        public:
			static std::shared_ptr<Engine> construct(const std::shared_ptr<system::Application>& sys_app);
            ~Engine();
			void update() override final;
			void terminate() override final;
			render::sync::Semaphore* create_semaphore() override final;
			render::texture::Texture2D* create_texture_2d(
				const core::Id id,
				const void *data,
				const render::texture::Format::Id f,
				const render::texture::Sample::Id s,
				const unsigned int width,
				const unsigned int heigt,
				const core::sync::EndCaller<core::sync::EndCallerIgnore> &call) override final;
			void submit(
				const std::vector<std::shared_ptr<render::sync::Semaphore>>& p, 
				const std::shared_ptr<render::command::Buffer>& c, 
				const std::shared_ptr<render::sync::Semaphore>& n) override final;
            //        render::texture::Texture2D* create_texture_2d(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //        render::texture::Cube* create_texture_cube(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //        render::buffer::Mesh* create_mesh(unsigned int vec, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //        render::buffer::Uniform* create_uniform(unsigned int s, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //        render::shader::Shader* create_shader(core::Id sid, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //        render::shader::Resources* create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //        render::pipeline::Pipeline* create_pipeline(core::Id sid, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //        void on_event(core::event::Event& e);
        };
    }
}
}
#endif
#endif

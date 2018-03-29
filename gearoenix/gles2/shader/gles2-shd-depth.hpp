#ifndef GEAROENIX_GLES2_SHADER_DEPTH_HPP
#define GEAROENIX_GLES2_SHADER_DEPTH_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/material/rnd-mat-depth.hpp"
#include "../gles2.hpp"
#include "gles2-shader.hpp"
namespace gearoenix {
namespace gles2 {
    namespace buffer {
        class Uniform;
    }
    namespace pipeline {
        class Pipeline;
    }
    namespace shader {
        class Depth : public Shader {
        public:
            class Resources : public render::material::Depth::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
            GLuint mvp;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLuint vtx_stride = 3 * sizeof(GLfloat);
            GLint vtx_att_ind;

        public:
            Depth(core::Id sid, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            ~Depth();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_mvp(const GLfloat* data);
        };
    }
}
}
#endif
#endif

#ifndef GEAROENIX_GLES3_SHADER_SKYBOX_BASIC_HPP
#define GEAROENIX_GLES3_SHADER_SKYBOX_BASIC_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/material/rnd-mat-skybox-basic.hpp"
#include "../gles3.hpp"
#include "gles3-shader.hpp"
namespace gearoenix {
namespace gles3 {
    namespace buffer {
        class Uniform;
    }
    namespace pipeline {
        class Pipeline;
    }
    namespace shader {
        class SkyboxBasic : public Shader {
        public:
            class Resources : public render::material::SkyboxBasic::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
            GLuint vp;
            GLuint ctxt;
            GLuint vtx_shd;
            GLuint frg_shd;

        public:
            SkyboxBasic(Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            ~SkyboxBasic();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_vp(const GLfloat* data);
        };
    }
}
}
#endif
#endif

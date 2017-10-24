#ifndef GEAROENIX_GLES2_SHADER_DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_NOSHADOW_OPAQUE
#define GEAROENIX_GLES2_SHADER_DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_NOSHADOW_OPAQUE
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/material/rnd-mat-directional-textured-speculated-nocube-fullshadow-opaque.hpp"
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
        class DirectionalTexturedSpeculatedNocubeNoshadowOpaque : public Shader {
        public:
            class Resources : public render::material::DirectionalTexturedSpeculatedNocubeFullshadowOpaque::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
            GLuint mvp;
            GLuint m;
            GLuint sun;
            GLuint sun_color;
            GLuint eye;
            GLuint spec_color;
            GLuint spec_factor;
            GLuint ambl_color;
            GLuint txt;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLint vtx_att_ind;
            GLint nrm_att_ind;
            GLint uv_att_ind;

        public:
            DirectionalTexturedSpeculatedNocubeNoshadowOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end);
            ~DirectionalTexturedSpeculatedNocubeNoshadowOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_mvp(const GLfloat* data);
            void set_m(const GLfloat* data);
            void set_sun(const GLfloat* data);
            void set_sun_color(const GLfloat* data);
            void set_eye(const GLfloat* data);
            void set_spec_color(const GLfloat* data);
            void set_spec_factor(GLfloat data);
            void set_ambl_color(const GLfloat* data);
        };
    }
}
}
#endif
#endif

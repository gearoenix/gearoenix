#ifndef GEAROENIX_gles3_SHADER_DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROENIX_gles3_SHADER_DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/material/rnd-mat-directional-colored-matte-nonreflective-shadowless-opaque.hpp"
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
        class DirectionalColoredMatteNonreflectiveShadowlessOpaque : public Shader {
        public:
            class Resources : public render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
            GLuint mvp;
            GLuint m;
            GLuint sun;
            GLuint sun_color;
            GLuint ambl_color;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLint vtx_att_ind;
            GLint nrm_att_ind;

        public:
            DirectionalColoredMatteNonreflectiveShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            ~DirectionalColoredMatteNonreflectiveShadowlessOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_ambl_color(const GLfloat* data);
            void set_mvp(const GLfloat* data);
            void set_m(const GLfloat* data);
            void set_sun(const GLfloat* data);
            void set_sun_color(const GLfloat* data);
        };
    }
}
}
#endif
#endif

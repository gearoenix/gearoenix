#ifndef GEAROENIX_gles3_SHADER_DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE_HPP
#define GEAROENIX_gles3_SHADER_DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/material/rnd-mat-directional-colored-speculated-baked-shadowless-opaque.hpp"
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
        class DirectionalColoredSpeculatedBakedShadowlessOpaque : public Shader {
        public:
            class Resources : public render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
            GLuint ambl_color;
            GLuint eye;
            GLuint m;
            GLuint rfl_fac;
            GLuint spec_color;
            GLuint spec_factors;
            GLuint sun;
            GLuint sun_color;
            GLuint vp;

            GLuint rfl_env;

            GLuint vtx_shd;
            GLuint frg_shd;

            GLint vtx_att_ind;
            GLint nrm_att_ind;

        public:
            DirectionalColoredSpeculatedBakedShadowlessOpaque(Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            ~DirectionalColoredSpeculatedBakedShadowlessOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_vp(const GLfloat* data);
            void set_m(const GLfloat* data);
            void set_sun(const GLfloat* data);
            void set_sun_color(const GLfloat* data);
            void set_eye(const GLfloat* data);
            void set_spec_color(const GLfloat* data);
            void set_spec_factors(const GLfloat* data);
            void set_ambl_color(const GLfloat* data);
            void set_rfl_fac(const GLfloat data);
        };
    }
}
}
#endif
#endif

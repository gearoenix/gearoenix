#ifndef GEAROENIX_GLES2_SHADER_DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE_HPP
#define GEAROENIX_GLES2_SHADER_DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/material/rnd-mat-directional-d2-speculated-nonreflective-full-opaque.hpp"
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
        class DirectionalD2SpeculatedNonreflectiveFullOpaque : public Shader {
        public:
            class Resources : public render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
            GLuint vp;
            GLuint m;
            GLuint sun;
            GLuint sun_color;
            GLuint eye;
            GLuint spec_color;
            GLuint spec_factors;
            GLuint ambl_color;
            GLuint db;
            GLuint txt;
            GLuint shdtxt;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLint vtx_att_ind;
            GLint nrm_att_ind;
            GLint uv_att_ind;

        public:
            DirectionalD2SpeculatedNonreflectiveFullOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end);
            ~DirectionalD2SpeculatedNonreflectiveFullOpaque();
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
            void set_db(const GLfloat* data);
        };
    }
}
}
#endif
#endif

#ifndef GEAROENIX_GLES2_SHADER_DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROENIX_GLES2_SHADER_DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../gles2.hpp"
#include "gles2-shader.hpp"
namespace gearoenix {
namespace gles2 {
    namespace shader {
        class DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque : public Shader {
        protected:
            GLuint mvp;
            GLuint m;
            GLuint sun;
            GLuint sun_color;
            GLuint eye;
            GLuint spec_color;
            GLuint spec_factor;
            GLuint color;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLint vtx_att_ind;
            GLint nrm_att_ind;

        public:
            DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end);
            ~DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_mvp(const GLfloat* data);
            void set_m(const GLfloat* data);
            void set_sun(const GLfloat* data);
            void set_sun_color(const GLfloat* data);
            void set_eye(const GLfloat* data);
            void set_spec_color(const GLfloat data);
            void set_spec_factor(GLfloat data);
        };
    }
}
}
#endif
#endif

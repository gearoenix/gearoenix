#ifndef GEAROENIX_GLES2_SHADER_DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROENIX_GLES2_SHADER_DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../gles2.hpp"
#include "gles2-shader.hpp"
namespace gearoenix {
namespace gles2 {
    namespace shader {
        class DirectionalColoredMatteNonreflectiveShadowlessOpaque : public Shader {
        protected:
            GLuint mvp;
            GLuint m;
            GLuint sun;
            GLuint color;
            GLuint ambl_color;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLint vtx_att_ind;
            GLint nrm_att_ind;

        public:
            DirectionalColoredMatteNonreflectiveShadowlessOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end);
            ~DirectionalColoredMatteNonreflectiveShadowlessOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_mvp(const GLfloat* data);
            void set_m(const GLfloat* data);
            void set_sun(const GLfloat* data);
            void set_color(const GLfloat data);
            void set_ambl_color(const GLfloat data);
        };
    }
}
}
#endif
#endif

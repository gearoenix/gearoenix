#ifndef GEAROENIX_GLES2_SHADER_DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_NOSHADOW_OPAQUE
#define GEAROENIX_GLES2_SHADER_DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_NOSHADOW_OPAQUE
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "gles2-shader.hpp"
#include <SDL2/SDL_opengles2.h>
namespace gearoenix {
namespace gles2 {
    namespace shader {
        class DirectionalTexturedSpeculatedNocubeNoshadowOpaque : public Shader {
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
            DirectionalTexturedSpeculatedNocubeNoshadowOpaque();
            ~DirectionalTexturedSpeculatedNocubeNoshadowOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            const render::shader::stage::Stage* get_stage(render::shader::stage::Id id) const;
            render::shader::stage::Stage* get_stage(render::shader::stage::Id id);
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

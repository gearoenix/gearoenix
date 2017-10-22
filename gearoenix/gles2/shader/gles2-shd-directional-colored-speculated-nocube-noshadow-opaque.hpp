#ifndef GEAROENIX_GLES2_SHADER_DIRECTIONAL_COLORED_SPECULATED_NOCUBE_NOSHADOW_OPAQUE
#define GEAROENIX_GLES2_SHADER_DIRECTIONAL_COLORED_SPECULATED_NOCUBE_NOSHADOW_OPAQUE
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "gles2-shader.hpp"
#include <SDL2/SDL_opengles2.h>
namespace gearoenix {
namespace gles2 {
    namespace shader {
        class DirectionalColoredSpeculatedNocubeNoshadowOpaque : public Shader {
        protected:
            GLuint mvp;
            GLuint m;
            GLuint sun;
            GLuint eye;
            GLuint color;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLint vtx_att_ind;
            GLint nrm_att_ind;

        public:
            DirectionalColoredSpeculatedNocubeNoshadowOpaque();
            ~DirectionalColoredSpeculatedNocubeNoshadowOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            const render::shader::stage::Stage* get_stage(render::shader::stage::Id id) const;
            render::shader::stage::Stage* get_stage(render::shader::stage::Id id);
            void set_mvp(const GLfloat* data);
        };
    }
}
}
#endif
#endif

#ifndef GEAROENIX_GLES3_SHADER_FONT_COLORED_HPP
#define GEAROENIX_GLES3_SHADER_FONT_COLORED_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/material/rnd-mat-font-colored.hpp"
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
        class FontColored : public Shader {
        public:
            class Resources : public render::material::FontColored::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
            GLuint mvp;
            GLuint color;
            GLuint txt2d;
            GLuint vtx_shd;
            GLuint frg_shd;
            GLint vtx_att_ind;
            GLint uv_att_ind;

        public:
            FontColored(core::Id my_id, Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            ~FontColored();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
            void set_mvp(const GLfloat* data);
            void set_color(const GLfloat* data);
        };
    }
}
}
#endif
#endif

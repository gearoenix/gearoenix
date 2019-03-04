//#ifndef GEAROENIX_GLES2_SHADER_SKYBOX_BASIC_HPP
//#define GEAROENIX_GLES2_SHADER_SKYBOX_BASIC_HPP
//#include "../../core/cr-build-configuration.hpp"
//#ifdef GX_USE_OPENGL_ES2
//#include "../../render/material/rnd-mat-skybox-basic.hpp"
//#include "../gles2.hpp"
//#include "gles2-shader.hpp"
//namespace gearoenix {
//namespace gles2 {
//    namespace buffer {
//        class Uniform;
//    }
//    namespace pipeline {
//        class Pipeline;
//    }
//    namespace shader {
//        class SkyboxBasic : public Shader {
//        public:
//            class Resources : public render::material::SkyboxBasic::Resources {
//            public:
//                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
//                void bind();
//            };

//        protected:
//            GLuint vp;
//            GLuint ctxt;
//            GLuint vtx_shd;
//            GLuint frg_shd;
//            GLint vtx_att_ind;

//        public:
//            SkyboxBasic(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
//            ~SkyboxBasic();
//            void use();
//            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
//            void set_vp(const GLfloat* data);
//        };
//    }
//}
//}
//#endif
//#endif

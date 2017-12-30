#ifndef GEAROENIX_gles3_SHADER_RESOURCES_HPP
#define GEAROENIX_gles3_SHADER_RESOURCES_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/shader/rnd-shd-resources.hpp"
namespace gearoenix {
namespace gles3 {
    class Engine;
    namespace buffer {
        class Uniform;
    }
    namespace pipeline {
        class Pipeline;
    }
    namespace texture {
        class Texture2D;
    }
    namespace shader {
        class Resources : public render::shader::Resources {
        private:
        public:
            Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            ~Resources();
            void bind();
        };
    }
}
}
#endif
#endif

#ifndef GEAROENIX_OPENGL_SHADER_RESOURCES_HPP
#define GEAROENIX_OPENGL_SHADER_RESOURCES_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
namespace gearoenix {
namespace render {
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
        class Resources {
        private:
        public:
            Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u, texture::Texture2D* t);
            ~Resources();
            void bind(pipeline::Pipeline* pip);
        };
    }
}
}
#endif
#endif

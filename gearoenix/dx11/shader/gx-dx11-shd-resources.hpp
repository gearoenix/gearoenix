#ifndef GEAROENIX_GLES2_SHADER_RESOURCES_HPP
#define GEAROENIX_GLES2_SHADER_RESOURCES_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/shader/gx-rnd-shd-resources.hpp"
namespace gearoenix {
namespace dx11 {
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

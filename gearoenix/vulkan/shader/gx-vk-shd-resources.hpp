#ifndef GEAROENIX_VULKAN_SHADER_RESOURCES_HPP
#define GEAROENIX_VULKAN_SHADER_RESOURCES_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
namespace gearoenix {
namespace render {
    class Engine;
    namespace buffer {
        class Uniform;
    }
    namespace descriptor {
        class Set;
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
            Engine* e;
            descriptor::Set** dessets;
            unsigned int dc;

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

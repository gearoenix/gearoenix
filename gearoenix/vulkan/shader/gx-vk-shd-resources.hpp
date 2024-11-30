#pragma once
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
namespace gearoenix {
namespace render {
    struct Engine;
    namespace buffer {
        struct Uniform;
    }
    namespace descriptor {
        struct Set;
    }
    namespace pipeline {
        struct Pipeline;
    }
    namespace texture {
        struct Texture2D;
    }
    namespace shader {
        struct Resources {
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
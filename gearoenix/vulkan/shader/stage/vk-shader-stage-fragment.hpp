#ifndef GEAROEMIX_VULKAN_SHADER_STAGE_FRAGMENT_HPP
#define GEAROEMIX_VULKAN_SHADER_STAGE_FRAGMENT_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "vk-shader-stage.hpp"
namespace gearoenix {
namespace render {
    namespace shader {
        namespace stage {
            class Fragment : public Stage {
            private:
            public:
                Fragment(std::vector<unsigned char>& data, Engine* engine);
            };
        }
    }
}
}
#endif
#endif

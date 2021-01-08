#ifndef GEAROENIX_VULKAN_SHADER_STAGE_FRAGMENT_HPP
#define GEAROENIX_VULKAN_SHADER_STAGE_FRAGMENT_HPP
#include "../../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "gx-vk-shader-stage.hpp"
namespace gearoenix {
namespace render {
    namespace shader {
        namespace stage {
            struct Fragment : public Stage {
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

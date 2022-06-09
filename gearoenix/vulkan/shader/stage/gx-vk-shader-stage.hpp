#ifndef GEAROENIX_VULKAN_SHADER_STAGE_STAGE_HPP
#define GEAROENIX_VULKAN_SHADER_STAGE_STAGE_HPP
#include "../../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../gx-vk-linker.hpp"
#include <vector>
namespace gearoenix {
namespace render {
    struct Engine;
    namespace device {
        struct Logical;
    }
    namespace shader {
        namespace stage {
            struct Stage {
            private:
                VkShaderModule vulkan_data;
                device::Logical* device;

            public:
                Stage(std::vector<unsigned char>& data, Engine* engine);
                ~Stage();
                VkShaderModule get_vulkan_data() const;
            };
        }
    }
}
}
#endif
#endif

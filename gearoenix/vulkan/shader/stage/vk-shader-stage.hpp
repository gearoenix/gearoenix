#ifndef GEAROENIX_VULKAN_SHADER_STAGE_STAGE_HPP
#define GEAROENIX_VULKAN_SHADER_STAGE_STAGE_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../vk-linker.hpp"
#include <vector>
namespace gearoenix {
namespace render {
    class Engine;
    namespace device {
        class Logical;
    }
    namespace shader {
        namespace stage {
            class Stage {
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

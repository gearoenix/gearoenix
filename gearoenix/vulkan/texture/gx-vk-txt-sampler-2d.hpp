#ifndef GEAROENIX_VULKAN_TEXTURE_SAMPLER_2D_HPP
#define GEAROENIX_VULKAN_TEXTURE_SAMPLER_2D_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../gx-vk-linker.hpp"

namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace texture {
        class Sampler2D {
        private:
            VkSampler vulkan_data;
            device::Logical* dev;

        public:
            Sampler2D(device::Logical* dev);
            ~Sampler2D();
            const VkSampler& get_vulkan_data() const;
        };
    }
}
}
#endif
#endif

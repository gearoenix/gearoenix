#ifndef GEAROENIX_VULKAN_PIPELINE_CACHE_HPP
#define GEAROENIX_VULKAN_PIPELINE_CACHE_HPP
#include "../vk-linker.hpp"

namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace pipeline {
        class Cache {
        private:
            device::Logical* logical_device;
            VkPipelineCache vulkan_data;

        public:
            Cache(device::Logical* logical_device);
            ~Cache();
            const device::Logical* get_logical_device() const;
            device::Logical* get_logical_device();
            const VkPipelineCache& get_vulkan_data() const;
        };
    } // namespace pipeline
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_PIPELINE_CACHE_HPP

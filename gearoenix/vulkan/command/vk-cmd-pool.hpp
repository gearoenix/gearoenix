#ifndef GEAROENIX_VULKAN_POOL_BUFFER_HPP
#define GEAROENIX_VULKAN_POOL_BUFFER_HPP
#include "../vk-linker.hpp"
#include <vector>
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace command {
        class Pool {
        private:
            device::Logical* logical_device;
            VkCommandPool vulkan_data;

        public:
            Pool(device::Logical* logical_device);
            ~Pool();
            const VkCommandPool& get_vulkan_data() const;
            const device::Logical* get_logical_device() const;
            device::Logical* get_logical_device();
        };
    }
}
}
#endif

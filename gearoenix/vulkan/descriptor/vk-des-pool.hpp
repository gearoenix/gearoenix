#ifndef GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace descriptor {
        class Pool {
        public:
            const static uint32_t TYPE_COUNT;

        private:
            device::Logical* logical_device;
            VkDescriptorPool vulkan_data;

        public:
            Pool(device::Logical* logical_device);
            ~Pool();
            const device::Logical* get_logical_device() const;
            device::Logical* get_logical_device();
            const VkDescriptorPool& get_vulkan_data() const;
        };
    } // namespace descriptor
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP

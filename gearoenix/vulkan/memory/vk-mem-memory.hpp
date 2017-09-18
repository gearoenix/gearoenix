#ifndef GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace memory {
        class Memory {
        public:
        private:
            device::Logical* logical_device;
            VkDeviceMemory vulkan_data;

        public:
            Memory(device::Logical* logical_device, VkDeviceSize size, uint32_t place, uint32_t memory_flags);
            ~Memory();
            const device::Logical* get_logical_device() const;
            device::Logical* get_logical_device();
            const VkDeviceMemory& get_vulkan_data();
        };
    } // namespace memory
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_MEMORY_MEMORY_HPP

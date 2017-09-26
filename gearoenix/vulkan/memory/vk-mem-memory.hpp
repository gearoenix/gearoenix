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
            bool is_in_gpu;

        public:
            Memory(device::Logical* logical_device, const VkMemoryRequirements& mem_req, uint32_t place = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            virtual ~Memory();
            const device::Logical* get_logical_device() const;
            device::Logical* get_logical_device();
            const VkDeviceMemory& get_vulkan_data() const;
            bool get_is_in_gpu() const;
        };
    } // namespace memory
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_MEMORY_MEMORY_HPP

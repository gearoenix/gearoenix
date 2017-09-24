#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace command {
        class Buffer;
    }
    namespace device {
        class Logical;
    }
    namespace memory {
        class Manager;
        class SubMemory;
    }
    namespace buffer {
        class Buffer {
        private:
            device::Logical* device;
            memory::SubMemory* smem;
            void* buffer_data;
            VkBuffer vulkan_data;

        public:
            Buffer(
                memory::Manager* mem_mgr, unsigned int size,
                uint32_t usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            ~Buffer();
            const VkBuffer& get_vulkan_data() const;
            void copy(command::Buffer* command, Buffer* src);
            void push_memory_barrier(command::Buffer* command) const;
            const device::Logical* get_logical_device() const;
            device::Logical* get_logical_device();
            static uint32_t get_memory_type_bits(device::Logical* device, bool in_gpu);
        };
    }
}
}
#endif

#ifndef GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#define GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../gx-vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace buffer {
        class SubBuffer;
    }
    namespace command {
        class Buffer;
    }
    namespace device {
        class Logical;
    }
    namespace memory {
        class SubMemory;
        class Manager;
        class Memory;
    }
    namespace image {
        class Image {
        private:
            device::Logical* logical_device;
            VkImage vulkan_data;
            memory::Memory* mem = nullptr;
            memory::SubMemory* submem = nullptr;
            uint32_t img_width = 0;
            uint32_t img_height = 0;
            VkFormat fmt = VK_FORMAT_UNDEFINED;

        public:
            Image(device::Logical* logical_device,
                const VkImage& vulkan_data,
                memory::Memory* mem = nullptr);
            Image(device::Logical* logical_device,
                const VkImageCreateInfo& info,
                memory::Manager* mem_manager = nullptr);
            ~Image();
            const VkImage& get_vulkan_data() const;
            const device::Logical* get_logical_device() const;
            void transit(command::Buffer* c, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt);
            void transit_for_writing(command::Buffer* c);
            void copy_from_buffer(command::Buffer* c, buffer::SubBuffer* b);
            void transit_for_reading(command::Buffer* c);
            VkFormat get_format() const;
        };
    }
}
}
#endif
#endif

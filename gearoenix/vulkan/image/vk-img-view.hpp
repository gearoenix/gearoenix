#ifndef GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#define GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace memory {
    }
    namespace device {
        class Logical;
    }
    namespace image {
        class Image;
        class View {
        private:
            Image* img;
            VkImageView vulkan_data;

        public:
            View(Image* img, const VkFormat& format);
            View(Image* img, const VkImageViewCreateInfo& info);
            ~View();
            static View* create_depth_stencil(device::Logical* logical_device);
            const Image* get_image() const;
            Image* get_image();
            const VkImageView& get_vulkan_data() const;
        };
    }
}
}
#endif
#endif

#ifndef GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#define GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#include "../vk-linker.hpp"
#include <memory>
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
            std::shared_ptr<Image> img;
            VkImageView vulkan_data;

        public:
            View(const std::shared_ptr<Image>& img, const VkFormat& format);
            View(const std::shared_ptr<Image>& img, const VkImageViewCreateInfo& info);
            ~View();
            static View* create_depth_stencil(device::Logical* logical_device);
            const std::shared_ptr<Image>& get_image() const;
            const VkImageView& get_vulkan_data() const;
        };
    }
}
}
#endif

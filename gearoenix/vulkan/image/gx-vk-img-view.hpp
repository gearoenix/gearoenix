#ifndef GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#define GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::memory {
class Manager;
}

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::image {
class Image;
class View {
private:
    GX_GET_REFC_PRV(std::shared_ptr<Image>, image)
    GX_GET_VAL_PRV(VkImageView, vulkan_data, nullptr)

public:
    View(std::shared_ptr<Image> img) noexcept;
    //            View(Image* img, const VkImageViewCreateInfo& info);
    ~View() noexcept;
    [[nodiscard]] static std::shared_ptr<View> create_depth_stencil(memory::Manager& mem_mgr) noexcept;
};
}
#endif
#endif

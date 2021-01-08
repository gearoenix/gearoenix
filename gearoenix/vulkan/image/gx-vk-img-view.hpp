#ifndef GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#define GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::image {
struct Image;
struct View {
private:
    GX_GET_REFC_PRV(std::shared_ptr<Image>, image)
    GX_GET_VAL_PRV(VkImageView, vulkan_data, nullptr)

public:
    explicit View(std::shared_ptr<Image> img) noexcept;
    ~View() noexcept;
    [[nodiscard]] static std::shared_ptr<View> create_depth_stencil(memory::Manager& mem_mgr) noexcept;
};
}
#endif
#endif

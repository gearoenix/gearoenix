#ifndef GEAROENIX_VULKAN_RENDER_PASS_HPP
#define GEAROENIX_VULKAN_RENDER_PASS_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan {
struct Swapchain;
struct RenderPass final {
    GX_GET_REFC_PRV(std::shared_ptr<Swapchain>, swapchain)
    GX_GET_VAL_PRV(VkRenderPass, vulkan_data, nullptr)

public:
    explicit RenderPass(std::shared_ptr<Swapchain> swapchain) noexcept;
    ~RenderPass() noexcept;
};
}
#endif
#endif

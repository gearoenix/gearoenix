#ifndef GEAROENIX_VULKAN_RENDER_PASS_HPP
#define GEAROENIX_VULKAN_RENDER_PASS_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../core/gx-cr-static.hpp"
#include "gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan {
class Swapchain;
class RenderPass final {
    GX_GET_REFC_PRV(std::shared_ptr<Swapchain>, swapchain)
    GX_GET_VAL_PRV(VkRenderPass, vulkan_data, nullptr)

public:
    explicit RenderPass(std::shared_ptr<Swapchain> swapchain) noexcept;
    ~RenderPass() noexcept;
};
}
#endif
#endif

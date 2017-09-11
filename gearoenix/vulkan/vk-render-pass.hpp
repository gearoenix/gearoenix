#ifndef GEAROENIX_NUFRAG_VULKAN_RENDER_PASS_HPP
#define GEAROENIX_NUFRAG_VULKAN_RENDER_PASS_HPP

#include <memory>
#include "vk-linker.hpp"

namespace gearoenix {
namespace nufrag {
namespace render {
namespace device {
class Logical;
}
class Swapchain;

class RenderPass {
private:
    std::shared_ptr<Swapchain> swapchain;
    VkRenderPass vulkan_data;
public:
    RenderPass(const std::shared_ptr<Swapchain> &swapchain);

    ~RenderPass();

    const VkRenderPass &get_vulkan_data() const;
};
}
}
}
#endif

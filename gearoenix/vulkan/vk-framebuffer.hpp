#ifndef GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#define GEAROENIX_VULKAN_FRAMEBUFFER_HPP

#include <memory>
#include "vk-linker.hpp"

namespace gearoenix {
namespace render {
namespace image {
class View;
}
class RenderPass;

class Framebuffer {
private:
    std::shared_ptr<image::View> view;
    std::shared_ptr<image::View> depth;
    std::shared_ptr<RenderPass> render_pass;
    VkFramebuffer vulkan_data;
public:
    Framebuffer(
            const std::shared_ptr<image::View> &view,
            const std::shared_ptr<image::View> &depth,
            const std::shared_ptr<RenderPass> &render_pass);

    ~Framebuffer();

    const VkFramebuffer &get_vulkan_data() const;
};
}
}
#endif

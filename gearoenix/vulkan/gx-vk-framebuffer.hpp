#ifndef GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#define GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "gx-vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace image {
        class View;
    }
    class RenderPass;

    class Framebuffer {
    private:
        image::View* view;
        image::View* depth;
        RenderPass* render_pass;
        VkFramebuffer vulkan_data;

    public:
        Framebuffer(image::View* view, image::View* depth, RenderPass* render_pass);
        ~Framebuffer();
        const VkFramebuffer& get_vulkan_data() const;
    };
}
}
#endif
#endif

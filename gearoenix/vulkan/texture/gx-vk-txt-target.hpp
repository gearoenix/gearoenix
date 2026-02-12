#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../render/texture/gx-rnd-txt-target.hpp"
#include "../gx-vk-loader.hpp"
#include "../image/gx-vk-img-image.hpp"

#include <memory>
#include <variant>
#include <vector>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::texture {
struct Texture2D;
struct TextureCube;

struct Target final : render::texture::Target {
    struct RenderingScope final {
    private:
        const VkCommandBuffer command_buffer;

    public:
        explicit RenderingScope(VkCommandBuffer cb);
        RenderingScope(RenderingScope&&) = delete;
        RenderingScope(const RenderingScope&) = delete;
        ~RenderingScope();
    };

    struct GapiAttachment final {
        constexpr static std::uint32_t VARIANT_2D_INDEX = 0;
        constexpr static std::uint32_t VARIANT_CUBE_INDEX = 1;

        std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>> texture;
        std::shared_ptr<image::View> view;
        image::TransitionRequest transition_request;
    };

    GX_GET_CREF_PRV(std::vector<GapiAttachment>, gapi_attachments);
    std::vector<VkRenderingAttachmentInfo> color_attachments;
    std::optional<VkRenderingAttachmentInfo> depth_attachment;
    VkRenderingInfo rendering_info {};

    Target(std::string&&, std::vector<render::texture::Attachment>&& attachments);

public:
    static void construct(
        std::string&& name, std::vector<render::texture::Attachment>&& attachments,
        core::job::EndCallerShared<render::texture::Target>&& end_callback);
    ~Target() override;
    [[nodiscard]] RenderingScope create_rendering_scope(VkCommandBuffer cb, VkAttachmentLoadOp load_colours = VK_ATTACHMENT_LOAD_OP_CLEAR, VkAttachmentLoadOp load_depth = VK_ATTACHMENT_LOAD_OP_CLEAR);
    void update_rendering_info();
};
}

#endif
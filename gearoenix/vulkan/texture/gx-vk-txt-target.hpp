#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
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
        const vk::CommandBuffer command_buffer;

    public:
        explicit RenderingScope(vk::CommandBuffer cb);
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

private:
    std::vector<GapiAttachment> gapi_attachments;
    std::vector<vk::RenderingAttachmentInfo> color_attachments;
    std::optional<vk::RenderingAttachmentInfo> depth_attachment;
    vk::RenderingInfo rendering_info { };

    Target(std::string&&, std::vector<render::texture::Attachment>&& attachments);

public:
    [[nodiscard]] const std::vector<GapiAttachment>& get_gapi_attachments() const { return gapi_attachments; }

    static void construct(std::string&& name, std::vector<render::texture::Attachment>&& attachments, core::job::EndCallerShared<render::texture::Target>&& end_callback);
    ~Target() override;
    [[nodiscard]] RenderingScope create_rendering_scope(vk::CommandBuffer cb, vk::AttachmentLoadOp load_colours = vk::AttachmentLoadOp::eClear, vk::AttachmentLoadOp load_depth = vk::AttachmentLoadOp::eClear);
    void update_rendering_info();
    [[nodiscard]] vk::Format get_colour_format() const;
};
}

#endif
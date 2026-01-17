#include "gx-vk-txt-target.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../render/texture/gx-rnd-txt-target.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "gx-vk-txt-2d.hpp"
#include "gx-vk-txt-3d.hpp"
#include "gx-vk-txt-cube.hpp"

gearoenix::vulkan::texture::Target::RenderingScope::RenderingScope(const VkCommandBuffer cb)
    : command_buffer(cb)
{
}

gearoenix::vulkan::texture::Target::RenderingScope::~RenderingScope()
{
    vkCmdEndRendering(command_buffer);
}

gearoenix::vulkan::texture::Target::Target(std::string&& in_name, std::vector<render::texture::Attachment>&& attachments)
    : render::texture::Target(std::move(in_name), std::move(attachments))
{
}

void gearoenix::vulkan::texture::Target::construct(
    std::string&& name,
    std::vector<render::texture::Attachment>&& attachments,
    core::job::EndCallerShared<render::texture::Target>&& end_callback)
{
    std::shared_ptr<Target> self(new Target(std::string(name), std::move(attachments)));
    self->update_rendering_info();
    end_callback.set_return(std::move(self));
}

gearoenix::vulkan::texture::Target::~Target() = default;


gearoenix::vulkan::texture::Target::RenderingScope gearoenix::vulkan::texture::Target::create_rendering_scope(const VkCommandBuffer cb) const
{
    vkCmdBeginRendering(cb, &rendering_info);
    return RenderingScope(cb);
}

void gearoenix::vulkan::texture::Target::update_rendering_info()
{
    GX_ASSERT_D(!attachments.empty());

    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;

    color_attachments.clear();
    color_attachments.reserve(attachments.size());

    depth_attachment.reset();

    gapi_attachments.clear();
    gapi_attachments.reserve(attachments.size());
    for (const auto& a : attachments) {
        gapi_attachments.emplace_back();
        auto& ga = gapi_attachments.back();

        const auto att_conv = [&] <std::size_t I, typename Tex> () {
            const auto& at = std::get<I>(a.var);
            auto texture = std::dynamic_pointer_cast<Tex>(at.txt);
            GX_ASSERT_D(nullptr != texture);

            const auto& view = *[&] {
                if constexpr (I == render::texture::Attachment::ATTACHMENT_2D_VARIANT_INDEX) {
                    GX_ASSERT_D(texture->get_mips().size() > a.mipmap_level);
                    return texture->get_mips()[a.mipmap_level].get();
                } else if constexpr (I == render::texture::Attachment::ATTACHMENT_CUBE_VARIANT_INDEX) {
                    GX_ASSERT_D(texture->get_mips()[static_cast<std::size_t>(at.face)].size() > a.mipmap_level);
                    return texture->get_mips()[static_cast<std::size_t>(at.face)][a.mipmap_level].get();
                }
                GX_UNEXPECTED;
            }();

            VkRenderingAttachmentInfo attachment_info;
            GX_SET_ZERO(attachment_info);
            attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachment_info.imageView = view.get_vulkan_data();
            attachment_info.imageLayout = view.get_layout();
            attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            const auto& info = texture->get_info();

            if (render::texture::format_is_depth(info.get_format())) {
                attachment_info.clearValue.depthStencil = VkClearDepthStencilValue { 1.0f, 0 };

                depth_attachment = attachment_info;
            } else {
                attachment_info.clearValue.color = VkClearColorValue { 0.0f, 0.0f, 0.0f, 0.0f };

                color_attachments.emplace_back(attachment_info);
            }

            GX_ASSERT_D(0 != info.get_width() && 0 != info.get_height());
            GX_ASSERT_D((rendering_info.renderArea.extent.width == 0 && rendering_info.renderArea.extent.height == 0)
                || (rendering_info.renderArea.extent.width == view.get_extent().width && rendering_info.renderArea.extent.height == view.get_extent().height));
            rendering_info.renderArea.extent.width = view.get_extent().width;
            rendering_info.renderArea.extent.height = view.get_extent().height;

            ga.texture = std::move(texture);
        };

        if (a.var.index() == render::texture::Attachment::ATTACHMENT_2D_VARIANT_INDEX) {
            att_conv.operator()<render::texture::Attachment::ATTACHMENT_2D_VARIANT_INDEX, Texture2D>();
        } else if (a.var.index() == render::texture::Attachment::ATTACHMENT_CUBE_VARIANT_INDEX) {
            att_conv.operator()<render::texture::Attachment::ATTACHMENT_CUBE_VARIANT_INDEX, TextureCube>();
        } else {
            GX_UNEXPECTED;
        }
    }

    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachments.size());
    rendering_info.pColorAttachments = color_attachments.data();
    rendering_info.pDepthAttachment = depth_attachment.has_value() ? &*depth_attachment : nullptr;
}

#endif
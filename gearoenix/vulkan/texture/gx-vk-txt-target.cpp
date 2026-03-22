#include "gx-vk-txt-target.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-target.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "gx-vk-txt-2d.hpp"
#include "gx-vk-txt-3d.hpp"
#include "gx-vk-txt-cube.hpp"

gearoenix::vulkan::texture::Target::RenderingScope::RenderingScope(const vk::CommandBuffer cb)
    : command_buffer(cb)
{
}

gearoenix::vulkan::texture::Target::RenderingScope::~RenderingScope() { command_buffer.endRendering(); }

gearoenix::vulkan::texture::Target::Target(std::string&& in_name, std::vector<render::texture::Attachment>&& attachments)
    : render::texture::Target(std::move(in_name), std::move(attachments))
{
}

void gearoenix::vulkan::texture::Target::construct(std::string&& name, std::vector<render::texture::Attachment>&& attachments, core::job::EndCallerShared<render::texture::Target>&& end_callback)
{
    std::shared_ptr<Target> self(new Target(std::string(name), std::move(attachments)));
    self->update_rendering_info();
    end_callback.set_return(std::move(self));
}

gearoenix::vulkan::texture::Target::~Target() = default;

gearoenix::vulkan::texture::Target::RenderingScope gearoenix::vulkan::texture::Target::create_rendering_scope(const vk::CommandBuffer cb, const vk::AttachmentLoadOp load_colours, const vk::AttachmentLoadOp load_depth)
{
    for (auto& ca : color_attachments) {
        ca.loadOp = load_colours;
    }
    if (depth_attachment.has_value()) {
        depth_attachment->loadOp = load_depth;
    }

    for (auto& [texture, view, transition_request] : gapi_attachments) {
        view->get_image()->transit(cb, transition_request);
    }

    cb.beginRendering(rendering_info);
    return RenderingScope(cb);
}

void gearoenix::vulkan::texture::Target::update_rendering_info()
{
    GX_ASSERT_D(!attachments.empty());

    rendering_info = vk::RenderingInfo { };

    color_attachments.clear();
    color_attachments.reserve(attachments.size());

    depth_attachment.reset();

    gapi_attachments.clear();
    gapi_attachments.reserve(attachments.size());
    for (const auto& a : attachments) {
        gapi_attachments.emplace_back();
        auto& [texture, view, transition_request] = gapi_attachments.back();

        const auto att_conv = [&]<std::size_t I, typename Tex>() {
            const auto& at = std::get<I>(a.var);
            auto vk_txt = std::dynamic_pointer_cast<Tex>(at.txt);
            GX_ASSERT_D(nullptr != vk_txt);

            int array_index = 0;
            int mip_index = 0;

            if constexpr (I == render::texture::Attachment::ATTACHMENT_2D_VARIANT_INDEX) {
                GX_ASSERT_D(vk_txt->get_mips().size() > a.mipmap_level);
                view = vk_txt->get_mips()[a.mipmap_level];
                array_index = 0;
                mip_index = a.mipmap_level;
            } else if constexpr (I == render::texture::Attachment::ATTACHMENT_CUBE_VARIANT_INDEX) {
                const auto face_index = static_cast<int>(at.face);
                GX_ASSERT_D(vk_txt->get_mips()[face_index].size() > a.mipmap_level);
                view = vk_txt->get_mips()[face_index][a.mipmap_level];
                array_index = face_index;
                mip_index = a.mipmap_level;
            } else {
                GX_UNEXPECTED;
            }

            const auto& info = vk_txt->get_info();
            const auto is_depth = render::texture::format_is_depth(info.get_format());

            transition_request = is_depth ? image::TransitionRequest::depth_attachment() : image::TransitionRequest::color_attachment();
            transition_request = transition_request.with_mips(mip_index, 1).with_layers(array_index, 1);

            vk::RenderingAttachmentInfo attachment_info;
            attachment_info.imageView = view->get_vulkan_data();
            attachment_info.imageLayout = is_depth ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eColorAttachmentOptimal;
            attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
            attachment_info.storeOp = vk::AttachmentStoreOp::eStore;

            if (is_depth) {
                attachment_info.clearValue.setDepthStencil({ 1.0f, 0 });
                depth_attachment = attachment_info;
            } else {
                attachment_info.clearValue.setColor(vk::ClearColorValue { std::array { 0.0f, 0.0f, 0.0f, 0.0f } });
                color_attachments.emplace_back(attachment_info);
            }

            const auto& extent = view->get_extent();

            GX_ASSERT_D(0 != info.get_width() && 0 != info.get_height());
            GX_ASSERT_D((rendering_info.renderArea.extent.width == 0 && rendering_info.renderArea.extent.height == 0) || (rendering_info.renderArea.extent.width == extent.width && rendering_info.renderArea.extent.height == extent.height));
            rendering_info.renderArea.extent.width = extent.width;
            rendering_info.renderArea.extent.height = extent.height;

            texture = std::move(vk_txt);
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
    rendering_info.setColorAttachments(color_attachments);
    rendering_info.pDepthAttachment = depth_attachment.has_value() ? &*depth_attachment : nullptr;
}

vk::Format gearoenix::vulkan::texture::Target::get_colour_format() const
{
    for (const auto& a : gapi_attachments) {
        if (a.transition_request.layout == vk::ImageLayout::eColorAttachmentOptimal) {
            return a.view->get_image()->get_format();
        }
    }
    return vk::Format::eUndefined;
}

#endif

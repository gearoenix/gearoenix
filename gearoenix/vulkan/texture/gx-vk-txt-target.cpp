#include "gx-vk-txt-target.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../image/gx-vk-img-view.hpp"
#include "gx-vk-txt-2d.hpp"
#include "gx-vk-txt-3d.hpp"
#include "gx-vk-txt-cube.hpp"

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
    GX_ASSERT_D(!self->attachments.empty());
    self->gapi_attachments.reserve(self->attachments.size());
    for (const auto& a : self->attachments) {
        self->gapi_attachments.emplace_back();
        auto& ga = self->gapi_attachments.back();
        if (a.var.index() == render::texture::Attachment::ATTACHMENT_2D_VARIANT_INDEX) {
            const auto& a2d = std::get<render::texture::Attachment::ATTACHMENT_2D_VARIANT_INDEX>(a.var);
            auto texture = std::dynamic_pointer_cast<Texture2D>(a2d.txt);
            auto& view = *texture->get_view();
            ga.view = view.get_vulkan_data();
            ga.extent = view.get_extent();
            ga.layout = view.get_layout();
            ga.texture = std::move(texture);
        } else if (a.var.index() == render::texture::Attachment::ATTACHMENT_CUBE_VARIANT_INDEX) {
            const auto& acb = std::get<render::texture::Attachment::ATTACHMENT_CUBE_VARIANT_INDEX>(a.var);
            auto texture = std::dynamic_pointer_cast<TextureCube>(acb.txt);
            auto& view = *texture->get_view();
            ga.view = view.get_vulkan_data();
            ga.extent = view.get_extent();
            ga.layout = view.get_layout();
            ga.texture = std::move(texture);
        } else {
            GX_UNEXPECTED;
        }
        if (format_is_depth(format)) {
            gla.binding_index = GL_DEPTH_ATTACHMENT;
        } else {
            gla.binding_index = current_binding_point;
            binding_points.push_back(current_binding_point);
            ++current_binding_point;
        }
    }
    end_callback.set_return(self);
    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [self = std::move(self), name = std::move(name), binding_points = std::move(binding_points), end_callback = std::move(end_callback)] {
        glGenFramebuffers(1, &self->framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, self->framebuffer);
        GX_GL_CHECK_D;
        for (auto& a : self->gl_attachments) {
            switch (a.texture.index()) {
            case GlAttachment::VARIANT_2D_INDEX:
                a.texture_object = std::get<GlAttachment::VARIANT_2D_INDEX>(a.texture)->get_object();
                break;
            case GlAttachment::VARIANT_CUBE_INDEX:
                a.texture_object = std::get<GlAttachment::VARIANT_CUBE_INDEX>(a.texture)->get_object();
                break;
            default:
                GX_UNEXPECTED;
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, a.binding_index, a.target, a.texture_object, a.mipmap_level);
            GX_GL_CHECK_D;
        }
        glDrawBuffers(static_cast<sizei>(binding_points.size()), binding_points.data());
        glReadBuffer(GL_NONE);
        GX_GL_CHECK_D;
        switch (const auto framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_UNDEFINED:
            GX_LOG_F("Target is the default framebuffer, but the default framebuffer does not exist.");
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            GX_LOG_F("One of the framebuffer attachment points are framebuffer incomplete.");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            GX_LOG_F("Framebuffer does not have at least one image attached to it");
        case GL_FRAMEBUFFER_UNSUPPORTED:
            GX_LOG_F("Depth and stencil attachments, if present, are not the same renderbuffer, "
                     "or if the combination of internal formats of the attached images violates "
                     "an implementation-dependent set of restrictions.");
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            GX_LOG_F("Incomplete/unsupported dimensions");
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            GX_LOG_F("Value of GL_RENDERBUFFER_SAMPLES is not the same for all attached "
                     "renderbuffers or, if the attached images are a mix of renderbuffers and "
                     "textures, the value of GL_RENDERBUFFER_SAMPLES is not zero. ");
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        default:
            GX_LOG_F("Unknown framebuffer error: " << framebuffer_status);
        }
        set_framebuffer_label(self->framebuffer, name);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

gearoenix::gl::Target::~Target()
{
    attachments.clear();
    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [f = framebuffer] {
        glDeleteFramebuffers(1, &f);
    });
}

void gearoenix::gl::Target::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

#endif
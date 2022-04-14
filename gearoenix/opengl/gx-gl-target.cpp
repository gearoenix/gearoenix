#include "gx-gl-target.hpp"

#include <utility>
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-loader.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::Target::Target(std::vector<Attachment> _attachments, const bool is_depth_necessary) noexcept
    : attachments(std::move(_attachments))
{
    GX_ASSERT_D(!attachments.empty());
#ifdef GX_DEBUG_MODE
    std::size_t number_of_specified_attachment_binding_points = 0;
    for (const auto& a : attachments)
        if (static_cast<enumerated>(-1) != a.binding_index)
            ++number_of_specified_attachment_binding_points;
    GX_ASSERT_D(number_of_specified_attachment_binding_points == 0 || attachments.size() == number_of_specified_attachment_binding_points);
#endif

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    colour_attachments.reserve(attachments.size());
    enumerated current_binding_point = GL_COLOR_ATTACHMENT0;
    bool has_depth = false;
    std::vector<enumerated> binding_points;
    GX_GL_CHECK_D;
    for (auto& a : attachments) {
        if (a.texture->get_info().format == render::texture::TextureFormat::D32 && a.texture->get_info().format == render::texture::TextureFormat::D24 && a.texture->get_info().format == render::texture::TextureFormat::D16) {
            GX_ASSERT_D(static_cast<enumerated>(-1) == a.binding_index || GL_DEPTH_ATTACHMENT == a.binding_index);
            a.binding_index = GL_DEPTH_ATTACHMENT;
            depth_attachment = a;
            GX_ASSERT_D(!has_depth); // Only one depth is allowed.
            has_depth = true;
        } else {
            if (static_cast<enumerated>(-1) == a.binding_index) {
                a.binding_index = current_binding_point;
                ++current_binding_point;
            }
            colour_attachments.push_back(a);
            binding_points.push_back(a.binding_index);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, a.binding_index, GL_TEXTURE_2D, a.texture->get_object(), a.mipmap_level);
        GX_GL_CHECK_D;
    }
    GX_GL_CHECK_D;
    glDrawBuffers(static_cast<sizei>(binding_points.size()), binding_points.data());
    if (!has_depth && is_depth_necessary) {
        const auto& info = attachments[0].texture->get_info();
        depth_render_buffer = 0;
        glGenRenderbuffers(1, &depth_render_buffer.value());
        glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer.value());
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, static_cast<sizei>(info.width), static_cast<sizei>(info.height));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer.value());
    } else {
        depth_render_buffer = std::nullopt;
    }

    const auto framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (framebuffer_status) {
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GX_GL_CHECK_D;
}

gearoenix::gl::Target::~Target() noexcept
{
    attachments.clear();
    colour_attachments.clear();
    depth_attachment = std::nullopt;
    glDeleteFramebuffers(1, &framebuffer);
}

void gearoenix::gl::Target::bind() noexcept
{
    if (depth_render_buffer.has_value())
        glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer.value());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}
#endif
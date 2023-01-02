#include "gx-gl-target.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-loader.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::Target::Target(Engine& e, std::vector<render::texture::Attachment>&& _attachments, const core::sync::EndCaller& end_callback) noexcept
    : render::texture::Target(std::move(_attachments))
    , e(e)
{
    GX_ASSERT_D(!attachments.empty());
    gl_attachments.reserve(attachments.size());
    std::vector<enumerated> binding_points;
    binding_points.reserve(attachments.size());
    enumerated current_binding_point = GL_COLOR_ATTACHMENT0;
    for (const auto& a : attachments) {
        gl_attachments.emplace_back();
        auto& gla = gl_attachments.back();
        gla.mipmap_level = static_cast<decltype(gla.mipmap_level)>(a.mipmap_level);
        auto format = render::texture::TextureFormat::Unknown;
        if (a.var.index() == 0) {
            const auto& a2d = std::get<0>(a.var);
            format = a2d.txt->get_info().format;
            gla.texture = std::dynamic_pointer_cast<Texture2D>(a2d.txt);
            gla.target = GL_TEXTURE_2D;
        } else if (a.var.index() == 1) {
            const auto& acb = std::get<1>(a.var);
            format = acb.txt->get_info().format;
            gla.texture = std::dynamic_pointer_cast<TextureCube>(acb.txt);
            gla.target = convert(acb.face);
        } else
            GX_UNEXPECTED;
        if (render::texture::format_is_depth(format)) {
            gla.binding_index = GL_DEPTH_ATTACHMENT;
        } else {
            gla.binding_index = current_binding_point;
            binding_points.push_back(current_binding_point);
            ++current_binding_point;
        }
    }
    e.todos.load([this, binding_points = std::move(binding_points), end_callback] {
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        GX_GL_CHECK_D;
        for (auto& a : gl_attachments) {
            a.texture_object = a.texture.index() == 0 ? std::get<0>(a.texture)->get_object() : std::get<1>(a.texture)->get_object();
            glFramebufferTexture2D(GL_FRAMEBUFFER, a.binding_index, a.target, a.texture_object, a.mipmap_level);
            GX_GL_CHECK_D;
        }
        glDrawBuffers(static_cast<sizei>(binding_points.size()), binding_points.data());
        glReadBuffer(GL_NONE);
        GX_GL_CHECK_D;
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
    });
}

gearoenix::gl::Target::~Target() noexcept
{
    attachments.clear();
    e.todos.load([framebuffer = framebuffer] {
        glDeleteFramebuffers(1, &framebuffer);
    });
}

void gearoenix::gl::Target::bind() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}
#endif
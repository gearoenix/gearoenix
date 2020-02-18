#include "glc3-txt-target-cube.hpp"

#include <utility>
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-cube.hpp"
#include "glc3-txt-sample.hpp"
#include "glc3-txt-target.hpp"

gearoenix::glc3::texture::TargetCube::TargetCube(
    const core::Id id,
    engine::Engine* const e,
    std::vector<std::pair<render::texture::Face, std::shared_ptr<TextureCube>>> face_textures) noexcept
    : render::texture::TargetCube(id, e)
    , base(new glc3::texture::Target(e))
    , face_textures(std::move(face_textures))
{
}

gearoenix::glc3::texture::TargetCube::~TargetCube() noexcept = default;

std::shared_ptr<gearoenix::glc3::texture::TargetCube> gearoenix::glc3::texture::TargetCube::construct(
    const core::Id id,
    engine::Engine* const e,
    std::vector<std::pair<render::texture::Face, std::shared_ptr<TextureCube>>> face_textures,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::vector<std::shared_ptr<gearoenix::render::texture::Texture>> textures(face_textures.size());
    for (std::size_t i = 0; i < face_textures.size(); ++i) {
        textures[i] = face_textures[i].second;
    }
    const std::shared_ptr<TargetCube> r(new TargetCube(id, e, std::move(face_textures)));
    e->get_function_loader()->load([r, call, textures { move(textures) }] {
        r->base->generate_framebuffer(textures, 0);
        for (std::size_t i = 0; i < result->texture_objects.size(); ++i) {
            const auto& txt_info = infos[i];
            const auto& txt_fmt = txt_info.f;
            const auto& txt = result->texture_objects[i];
            if (txt_fmt == render::texture::TextureFormat::D32) {
                gl::Loader::bind_texture(GL_TEXTURE_2D, txt);
                gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, result->img_width, result->img_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                /// The nearest filter in here is for workaround for some buggy vendors
                gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, txt, 0);
            } else {
                GXUNIMPLEMENTED
            }
        }
        // TODO: remake the log fatal again
        if (gl::Loader::check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            GXLOGF("Failed to create render target!")
        result->state_init();
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
    });
    return result;
}

void gearoenix::glc3::texture::TargetCube::bind() const noexcept
{
    if (-1 != depth_buffer)
        gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    //    gl::Loader::viewport(0, 0, static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
    //    gl::Loader::scissor(0, 0, static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
    gl::Loader::enable(GL_DEPTH_TEST);
    gl::Loader::depth_mask(GL_TRUE);
    if (!texture_objects.empty())
        gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    else
        gl::Loader::clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void gearoenix::glc3::texture::TargetCube::bind_textures(const std::vector<gl::enumerated>& texture_units) const noexcept
{
#ifdef GX_DEBUG_MODE
    if (texture_units.size() != texture_objects.size())
        GXLOGF("Different size for texture units is not acceptable.")
#endif
    for (std::size_t i = 0; i < texture_objects.size(); ++i) {
        gl::Loader::active_texture(GL_TEXTURE0 + texture_units[i]);
        gl::Loader::bind_texture(GL_TEXTURE_2D, texture_objects[i]);
    }
}

#endif

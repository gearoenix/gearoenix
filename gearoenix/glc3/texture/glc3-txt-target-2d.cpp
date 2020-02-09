#include "glc3-txt-target-2d.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-sample.hpp"

gearoenix::glc3::texture::Target2D::Target2D(const core::Id id, engine::Engine* const e) noexcept
    : render::texture::Texture(id, render::texture::Type::Target2D, e)
    , render::texture::Target2D(id, e)
    , glc3::texture::Target(e)
{
}

gearoenix::glc3::texture::Target2D::Target2D(engine::Engine* const e) noexcept
    : render::texture::Texture(core::asset::Manager::create_id(), render::texture::Type::Target2D, e)
    , render::texture::Target2D(asset_id, e)
    , glc3::texture::Target(e)
{
    const auto* const sys_app = e->get_system_application();
    img_width = sys_app->get_window_width();
    img_height = sys_app->get_window_height();
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
    state_init();
}

gearoenix::glc3::texture::Target2D::~Target2D() noexcept = default;

std::shared_ptr<gearoenix::glc3::texture::Target2D> gearoenix::glc3::texture::Target2D::construct(
    core::Id my_id,
    engine::Engine* e,
    const std::vector<render::texture::Info>& infos,
    unsigned int w,
    unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<Target2D> result(new Target2D(my_id, e));
    result->img_width = w;
    result->img_height = h;
    result->texture_objects.resize(infos.size());
    e->get_function_loader()->load([result, infos, call] {
        gl::Loader::gen_framebuffers(1, reinterpret_cast<gl::uint*>(&(result->framebuffer)));
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, result->framebuffer);
        gl::Loader::gen_textures(static_cast<gearoenix::gl::sizei>(result->texture_objects.size()), result->texture_objects.data());
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
        if (gl::Loader::check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            GXLOGF("Failed to create render target!")
        result->state_init();
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
    });
    return result;
}

#endif

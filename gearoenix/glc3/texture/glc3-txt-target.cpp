#include "glc3-txt-target.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "../glc3.hpp"
#include "glc3-txt-sample.hpp"

#ifdef GX_DEBUG_GL_CLASS_3
#define GX_DEBUG_GL_CLASS_3_TARGET
#endif

void gearoenix::glc3::texture::Target::state_init() const noexcept
{
    gl::Loader::clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    gl::Loader::enable(GL_CULL_FACE);
    gl::Loader::cull_face(GL_BACK);
    gl::Loader::enable(GL_BLEND);
    gl::Loader::blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl::Loader::enable(GL_DEPTH_TEST);
    gl::Loader::enable(GL_SCISSOR_TEST);
    gl::Loader::enable(GL_STENCIL_TEST);
    gl::Loader::viewport(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
    gl::Loader::scissor(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
}

gearoenix::glc3::texture::Target::Target(engine::Engine* const e) noexcept
    : render::texture::Target(core::asset::Manager::create_id(), e)
{
    const auto* sys_app = e->get_system_application();
    img_width = sys_app->get_width();
    img_height = sys_app->get_height();
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
    state_init();
}

gearoenix::glc3::texture::Target::Target(
    core::Id my_id,
    engine::Engine* e,
    const std::vector<render::texture::Info>& infos,
    unsigned int w,
    unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : render::texture::Target(my_id, e)
{
    img_width = w;
    img_height = h;
    texture_objects.resize(infos.size());
    e->get_function_loader()->load([this, infos, call] {
        gl::Loader::gen_framebuffers(1, reinterpret_cast<gl::uint*>(&framebuffer));
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
        gl::Loader::gen_textures(texture_objects.size(), texture_objects.data());
        for (std::size_t i = 0; i < texture_objects.size(); ++i) {
            const auto& txt_info = infos[i];
            const auto& txt_fmt = txt_info.f;
            const auto& txt = texture_objects[i];
            if (txt_fmt == render::texture::TextureFormat::D_32) {
                gl::Loader::bind_texture(GL_TEXTURE_2D, txt);
                gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, img_width, img_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        state_init();
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
    });
}

gearoenix::glc3::texture::Target::~Target() noexcept
{
    if (texture_objects.size() == 0) // This is main render-target
        return;
    const auto cf = framebuffer;
    const auto cr = depth_buffer;
    render_engine->get_function_loader()->load([cf, cr, txts { move(texture_objects) }] {
        if (cf != -1)
            gl::Loader::delete_framebuffers(1, reinterpret_cast<const gl::uint*>(&cf));
        if (cr != -1)
            gl::Loader::delete_renderbuffers(1, reinterpret_cast<const gl::uint*>(&cr));
        for (auto ct : txts)
            gl::Loader::delete_textures(1, &ct);
    });
}

void gearoenix::glc3::texture::Target::bind() const noexcept
{
    if (-1 != depth_buffer)
        gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    gl::Loader::viewport(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
    gl::Loader::scissor(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
    gl::Loader::enable(GL_DEPTH_TEST);
    gl::Loader::depth_mask(GL_TRUE);
    if (texture_objects.size() != 0)
        gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    else
        gl::Loader::clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void gearoenix::glc3::texture::Target::bind_textures(const std::vector<gl::enumerated>& texture_units) const noexcept
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
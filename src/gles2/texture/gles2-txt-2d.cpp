#include "gles2-txt-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-sample.hpp"

static constexpr auto GX_GLES2_MIN_TEX2D_ASPECT = 64;

gearoenix::gles2::texture::Texture2D::Texture2D(
    const core::Id my_id,
    const std::shared_ptr<engine::Engine>& e,
    const void* data,
    const render::texture::TextureFormat::Id f,
    const render::texture::SampleInfo s,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
    : render::texture::Texture2D(my_id, e)
{
    const SampleInfo sample_info = SampleInfo(s);
    gl::uint cf;
    const gl::sizei gimg_width = GX_GLES2_MIN_TEX2D_ASPECT < img_width ? static_cast<gl::sizei>(img_width) : GX_GLES2_MIN_TEX2D_ASPECT;
    const gl::sizei gimg_height = GX_GLES2_MIN_TEX2D_ASPECT < img_height ? static_cast<gl::sizei>(img_height) : GX_GLES2_MIN_TEX2D_ASPECT;
#ifdef GX_DEBUG_GLES2
    if ((img_width != 1 && img_width < GX_GLES2_MIN_TEX2D_ASPECT) || (img_height != 1 && img_height < GX_GLES2_MIN_TEX2D_ASPECT))
        GXLOGF("Unsupported image aspect in GLES2 for texture id: " << my_id);
#endif
    std::uint8_t* pixels = nullptr;
    if (f == render::texture::TextureFormat::RGBA_FLOAT32 && img_width == 1 && img_height == 1) {
        cf = GL_RGBA;
        const gl::sizei pixel_size = gimg_width * gimg_height * 4;
        const core::Real* rdata = reinterpret_cast<const core::Real*>(data);
        pixels = new std::uint8_t[pixel_size];
        pixels[0] = static_cast<std::uint8_t>(rdata[0] * 255.1f);
        pixels[1] = static_cast<std::uint8_t>(rdata[1] * 255.1f);
        pixels[2] = static_cast<std::uint8_t>(rdata[2] * 255.1f);
        pixels[3] = static_cast<std::uint8_t>(rdata[3] * 255.1f);
        for (gl::sizei i = 4; i < pixel_size;)
            for (int j = 0; j < 4; ++j, ++i)
                pixels[i] = pixels[j];
    } else
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << my_id);
    e->get_function_loader()->load([this, pixels, cf, gimg_width, gimg_height, sample_info, call] {
        gl::Loader::gen_textures(1, &texture_object);
        gl::Loader::bind_texture(GL_TEXTURE_2D, texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, static_cast<gl::sint>(cf), gimg_width, gimg_height, 0, static_cast<gl::enumerated>(cf), GL_UNSIGNED_BYTE, pixels);
        gl::Loader::generate_mipmap(GL_TEXTURE_2D);
        gl::Loader::check_for_error();
        delete[] pixels;
    });
}

gearoenix::gles2::texture::Texture2D::Texture2D(const core::Id my_id, const gl::uint txtobj, const std::shared_ptr<engine::Engine>& e)
    : render::texture::Texture2D(my_id, e)
    , texture_object(txtobj)
{
}

gearoenix::gles2::texture::Texture2D::~Texture2D()
{
    if (texture_object == 0)
        return;
    const gl::uint c_texture_object = texture_object;
    render_engine->get_function_loader()->load([c_texture_object] {
        gl::Loader::bind_texture(GL_TEXTURE_2D, 0);
        gl::Loader::delete_textures(1, &c_texture_object);
    });
    texture_object = 0;
}

void gearoenix::gles2::texture::Texture2D::bind(gl::enumerated texture_unit) const
{
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    gl::Loader::bind_texture(GL_TEXTURE_2D, texture_object);
}

#endif

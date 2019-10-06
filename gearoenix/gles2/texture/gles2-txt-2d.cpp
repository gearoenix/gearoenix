#include "gles2-txt-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-sample.hpp"

gearoenix::gles2::texture::Texture2D::Texture2D(const core::Id my_id, engine::Engine* const e) noexcept
    : render::texture::Texture2D(my_id, e) {}

std::shared_ptr<gearoenix::gles2::texture::Texture2D> gearoenix::gles2::texture::Texture2D::construct(
    const core::Id my_id,
    engine::Engine* const e,
    const void* const data,
    const render::texture::TextureFormat::Id f,
    const render::texture::SampleInfo s,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(my_id, e));
    const SampleInfo sample_info = SampleInfo(s);
    gl::uint cf;
    const gl::sizei gimg_width = static_cast<gl::sizei>(img_width);
    const gl::sizei gimg_height = static_cast<gl::sizei>(img_height);
    std::vector<std::uint8_t> pixels;
    if (f == render::texture::TextureFormat::RGBA_FLOAT32) {
        cf = GL_RGBA;
        const gl::sizei pixel_size = gimg_width * gimg_height << 2;
        pixels.resize(pixel_size);
        const auto rdata = reinterpret_cast<const core::Real*>(data);
        for (gl::sizei i = 0; i < pixel_size; ++i) {
            const auto c = rdata[i] * 255.1f;
            if (c >= 255.0f)
                pixels[i] = 255;
            else if (c < 0.0f)
                pixels[i] = 0;
            else
                pixels[i] = static_cast<std::uint8_t>(c);
        }
    } else if (f == render::texture::TextureFormat::RGBA_UINT8) {
        cf = GL_RGBA;
        const gl::sizei pixel_size = gimg_width * gimg_height << 2;
        pixels.resize(pixel_size);
        const auto rdata = reinterpret_cast<const std::uint8_t*>(data);
        for (gl::sizei i = 0; i < pixel_size; ++i)
            pixels[i] = rdata[i];
    } else
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << my_id)
    e->get_function_loader()->load([result, pixels { move(pixels) }, cf, gimg_width, gimg_height, sample_info, call] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, static_cast<gl::sint>(cf), gimg_width, gimg_height, 0, static_cast<gl::enumerated>(cf), GL_UNSIGNED_BYTE, pixels.data());
        gl::Loader::generate_mipmap(GL_TEXTURE_2D);
#ifdef GX_DEBUG_GLES2
        gl::Loader::check_for_error();
#endif
    });
}

gearoenix::gles2::texture::Texture2D::Texture2D(const core::Id my_id, const gl::uint txtobj, engine::Engine* const e) noexcept
    : render::texture::Texture2D(my_id, e)
    , texture_object(txtobj)
{
}

gearoenix::gles2::texture::Texture2D::~Texture2D() noexcept
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

void gearoenix::gles2::texture::Texture2D::bind(gl::enumerated texture_unit) const noexcept
{
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    gl::Loader::bind_texture(GL_TEXTURE_2D, texture_object);
}

#endif

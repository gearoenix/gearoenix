#include "glc3-txt-2d.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-sample.hpp"

gearoenix::glc3::texture::Texture2D::Texture2D(const core::Id my_id, engine::Engine* const e) noexcept
    : render::texture::Texture(my_id, render::texture::Type::Texture2D, e)
    , render::texture::Texture2D(my_id, e)
{
}

std::shared_ptr<gearoenix::glc3::texture::Texture2D> gearoenix::glc3::texture::Texture2D::construct(
    const core::Id my_id,
    engine::Engine* const e,
    const void* const data,
    const render::texture::TextureFormat f,
    const render::texture::SampleInfo s,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(my_id, e));
    const SampleInfo sample_info = SampleInfo(s);
    const bool needs_mipmap = sample_info.needs_mipmap();
    gl::enumerated cf;
    gl::sint tf;
    gl::enumerated ef;
    const auto gimg_width = static_cast<gl::sizei>(img_width);
    const auto gimg_height = static_cast<gl::sizei>(img_height);
    std::vector<std::uint8_t> pixels;
    switch (f) {
    case render::texture::TextureFormat::RgbaFloat32:
        // TODO: I can in future check for support of format, if it does not support, convert it
        cf = GL_RGBA;
        tf = GL_RGBA32F;
        ef = GL_FLOAT;
        pixels.resize(gimg_width * gimg_height * 4 * 4);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgbFloat32:
        // TODO: I can in future check for support of format, if it does not support, convert it
        cf = GL_RGB;
        tf = GL_RGB32F;
        ef = GL_FLOAT;
        pixels.resize(gimg_width * gimg_height * 3 * 4);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgbFloat16:
        // TODO: I can in future check for support of format, if it does not support, convert it
        cf = GL_RGB;
        tf = GL_RGB16F;
        ef = GL_HALF_FLOAT;
        pixels.resize(gimg_width * gimg_height * 3 * 2);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgbaUint8:
        cf = GL_RGBA;
        tf = GL_RGBA;
        ef = GL_UNSIGNED_BYTE;
        pixels.resize(gimg_width * gimg_height * 4);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    default:
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << my_id)
    }
    e->get_function_loader()->load([result, needs_mipmap, pixels { move(pixels) }, tf, cf, ef, gimg_width, gimg_height, sample_info, call] {
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, tf, gimg_width, gimg_height, 0, cf, ef, pixels.data());
        if (needs_mipmap)
            gl::Loader::generate_mipmap(GL_TEXTURE_2D);
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
    });
    return result;
}

gearoenix::glc3::texture::Texture2D::Texture2D(const core::Id my_id, const gl::uint txt_obj, engine::Engine* const e) noexcept
    : render::texture::Texture(my_id, render::texture::Type::Texture2D, e)
    , render::texture::Texture2D(my_id, e)
    , texture_object(txt_obj)
{
}

gearoenix::glc3::texture::Texture2D::~Texture2D() noexcept
{
    if (texture_object == 0)
        return;
    const gl::uint c_texture_object = texture_object;
    render_engine->get_function_loader()->load([c_texture_object] {
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        gl::Loader::bind_texture(GL_TEXTURE_2D, 0);
        gl::Loader::delete_textures(1, &c_texture_object);
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
    });
    texture_object = 0;
}

void gearoenix::glc3::texture::Texture2D::bind(gl::enumerated texture_unit) const noexcept
{
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    gl::Loader::bind_texture(GL_TEXTURE_2D, texture_object);
}

#endif

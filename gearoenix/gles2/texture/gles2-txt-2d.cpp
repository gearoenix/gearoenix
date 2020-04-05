#include "gles2-txt-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-sample.hpp"

gearoenix::gles2::texture::Texture2D::Texture2D(
    const core::Id id,
    const render::texture::TextureFormat texture_format,
    engine::Engine* const e) noexcept
    : render::texture::Texture2D(id, texture_format, e)
{
}

std::shared_ptr<gearoenix::gles2::texture::Texture2D> gearoenix::gles2::texture::Texture2D::construct(
    const core::Id id,
    engine::Engine* const e,
    const void* const data,
    const render::texture::TextureInfo& info,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<Texture2D> result(new Texture2D(id, info.format, e));
    result->img_width = img_width;
    result->img_height = img_height;
    const SampleInfo sample_info(info.sample_info);
    const bool needs_mipmap = info.has_mipmap;
    const auto cf = convert(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(img_width);
    const auto gl_img_height = static_cast<gl::sizei>(img_height);
    std::vector<std::uint8_t> pixels;
    switch (info.format) {
    case render::texture::TextureFormat::RgbaFloat32: {
        const gl::sizei pixel_size = gl_img_width * gl_img_height * 4;
        pixels.resize(pixel_size);
        const auto raw_data = reinterpret_cast<const float*>(data);
        for (gl::sizei i = 0; i < pixel_size; ++i) {
            const auto c = raw_data[i] * 255.1f;
            if (c >= 255.0f)
                pixels[i] = 255;
            else if (c < 0.0f)
                pixels[i] = 0;
            else
                pixels[i] = static_cast<std::uint8_t>(c);
        }
        break;
    }
    case render::texture::TextureFormat::RgbFloat32: {
        const gl::sizei pixel_size = gl_img_width * gl_img_height * 3;
        pixels.resize(pixel_size);
        const auto raw_data = reinterpret_cast<const float*>(data);
        for (gl::sizei i = 0; i < pixel_size; ++i) {
            const auto c = raw_data[i] * 255.1f;
            if (c >= 255.0f)
                pixels[i] = 255;
            else if (c < 0.0f)
                pixels[i] = 0;
            else
                pixels[i] = static_cast<std::uint8_t>(c);
        }
        break;
    }
    case render::texture::TextureFormat::RgFloat32: {
        const gl::sizei pixel_size = gl_img_width * gl_img_height * 3;
        pixels.resize(pixel_size);
        const auto raw_data = reinterpret_cast<const float*>(data);
        for (gl::sizei i = 0, di = 0; i < pixel_size; ++i) {
            for (int j = 0; j < 2; ++j, ++di, ++i) {
                const auto c = raw_data[di] * 255.1f;
                if (c >= 255.0f)
                    pixels[i] = 255;
                else if (c < 0.0f)
                    pixels[i] = 0;
                else
                    pixels[i] = static_cast<std::uint8_t>(c);
            }
            pixels[i] = 0;
        }
        break;
    }
    case render::texture::TextureFormat::RgbaUint8: {
        const gl::sizei pixel_size = gl_img_width * gl_img_height * 4;
        pixels.resize(pixel_size);
        const auto raw_data = reinterpret_cast<const std::uint8_t*>(data);
        for (gl::sizei i = 0; i < pixel_size; ++i)
            pixels[i] = raw_data[i];
        break;
    }
    default:
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << id)
    }
    e->get_function_loader()->load([needs_mipmap, result, pixels { move(pixels) }, cf, gl_img_width, gl_img_height, sample_info, call] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, static_cast<gl::sint>(cf), gl_img_width, gl_img_height, 0, static_cast<gl::enumerated>(cf), GL_UNSIGNED_BYTE, pixels.data());
        if (needs_mipmap)
            gl::Loader::generate_mipmap(GL_TEXTURE_2D);
#ifdef GX_DEBUG_GLES2
        gl::Loader::check_for_error();
#endif
    });
    return result;
}

std::shared_ptr<gearoenix::gles2::texture::Texture2D> gearoenix::gles2::texture::Texture2D::construct(
    const core::Id id,
    engine::Engine* const e,
    const render::texture::TextureInfo& info,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(id, info.format, e));
    result->img_width = img_width;
    result->img_height = img_height;
    const SampleInfo sample_info(info.sample_info);
    const bool needs_mipmap = info.has_mipmap;
    const auto cf = convert(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(img_width);
    const auto gl_img_height = static_cast<gl::sizei>(img_height);
    e->get_function_loader()->load([needs_mipmap, result, cf, gl_img_width, gl_img_height, sample_info, call] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        gl::Loader::tex_image_2d(
            GL_TEXTURE_2D, 0, static_cast<gl::sint>(cf), gl_img_width, gl_img_height, 0,
            static_cast<gl::enumerated>(cf), GL_UNSIGNED_BYTE, nullptr);
        if (needs_mipmap)
            gl::Loader::generate_mipmap(GL_TEXTURE_2D);
#ifdef GX_DEBUG_GLES2
        gl::Loader::check_for_error();
#endif
    });
    return result;
}

//gearoenix::gles2::texture::Texture2D::Texture2D(const core::Id my_id, const gl::uint txt_obj, engine::Engine* const e) noexcept
//    : render::texture::Texture2D(my_id, e)
//    , texture_object(txt_obj)
//{
//}

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

void gearoenix::gles2::texture::Texture2D::write_gx3d(
    const std::shared_ptr<system::stream::Stream>&,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::gles2::texture::Texture2D::bind(gl::enumerated texture_unit) const noexcept
{
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    bind();
}

void gearoenix::gles2::texture::Texture2D::bind() const noexcept
{
    gl::Loader::bind_texture(GL_TEXTURE_2D, texture_object);
}

gearoenix::gl::uint gearoenix::gles2::texture::Texture2D::convert(gearoenix::render::texture::TextureFormat format) noexcept
{
    switch (format) {
    case render::texture::TextureFormat::RgbaFloat32:
        return GL_RGBA;
    case render::texture::TextureFormat::RgbFloat32:
    case render::texture::TextureFormat::RgFloat32:
        return GL_RGB;
    case render::texture::TextureFormat::RgbaUint8:
        return GL_RGBA;
    case render::texture::TextureFormat::D16:
        return GL_RGB;
    default:
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << static_cast<int>(format))
    }
}

#endif

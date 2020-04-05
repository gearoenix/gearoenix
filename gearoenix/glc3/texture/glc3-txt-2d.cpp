#include "glc3-txt-2d.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-sample.hpp"

gearoenix::glc3::texture::Texture2D::Texture2D(const core::Id id, const render::texture::TextureFormat texture_format, engine::Engine* const e) noexcept
    : render::texture::Texture2D(id, texture_format, e)
{
}

std::shared_ptr<gearoenix::glc3::texture::Texture2D> gearoenix::glc3::texture::Texture2D::construct(
    const core::Id my_id,
    engine::Engine* const e,
    const void* const data,
    const render::texture::TextureInfo& info,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(my_id, info.format, e));
    result->img_width = img_width;
    result->img_height = img_height;
    const SampleInfo sample_info(info.sample_info);
    const bool needs_mipmap = info.has_mipmap;
    const auto internal_format = convert_internal_format(info.format);
    const auto format = convert_format(info.format);
    const auto data_format = convert_data_format(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(img_width);
    const auto gl_img_height = static_cast<gl::sizei>(img_height);
    std::vector<std::uint8_t> pixels;
    switch (info.format) {
    case render::texture::TextureFormat::RgbaFloat32:
        // TODO: I can in future check for support of format, if it does not support, convert it
        pixels.resize(gl_img_width * gl_img_height * 4 * 4);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgbFloat32:
        // TODO: I can in future check for support of format, if it does not support, convert it
        pixels.resize(gl_img_width * gl_img_height * 3 * 4);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgFloat32:
        // TODO: I can in future check for support of format, if it does not support, convert it
        pixels.resize(gl_img_width * gl_img_height * 2 * 4);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgbFloat16:
        // TODO: I can in future check for support of format, if it does not support, convert it
        pixels.resize(gl_img_width * gl_img_height * 3 * 2);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgbaUint8:
        pixels.resize(gl_img_width * gl_img_height * 4);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    case render::texture::TextureFormat::RgbUint8:
        pixels.resize(gl_img_width * gl_img_height * 3);
        std::memcpy(pixels.data(), data, pixels.size());
        break;
    default:
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << my_id)
    }
    e->get_function_loader()->load([result, needs_mipmap, pixels { move(pixels) }, internal_format, format, data_format, gl_img_width, gl_img_height, sample_info, call] {
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, internal_format, gl_img_width, gl_img_height, 0, format, data_format, pixels.data());
        if (needs_mipmap)
            gl::Loader::generate_mipmap(GL_TEXTURE_2D);
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
    });
    return result;
}

std::shared_ptr<gearoenix::glc3::texture::Texture2D> gearoenix::glc3::texture::Texture2D::construct(
    const core::Id my_id,
    engine::Engine* const e,
    const render::texture::TextureInfo& info,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(my_id, info.format, e));
    result->img_width = img_width;
    result->img_height = img_height;
    const SampleInfo sample_info(info.sample_info);
    const bool needs_mipmap = info.has_mipmap;
    const auto internal_format = convert_internal_format(info.format);
    const auto format = convert_format(info.format);
    const auto data_format = convert_data_format(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(img_width);
    const auto gl_img_height = static_cast<gl::sizei>(img_height);
    auto* const zero_data = new std::uint32_t[img_width * img_height * 4];
    for (unsigned int i = 0, di = 0; i < img_width; ++i)
        for (unsigned int j = 0; j < img_height; ++j, ++di)
            zero_data[di] = 0;
    e->get_function_loader()->load([result, needs_mipmap, internal_format, format, data_format, gl_img_width, gl_img_height, sample_info, call, zero_data] {
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, internal_format, gl_img_width, gl_img_height, 0, format, data_format, zero_data);
        if (needs_mipmap)
            gl::Loader::generate_mipmap(GL_TEXTURE_2D);
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        delete[] zero_data;
    });
    return result;
}

gearoenix::glc3::texture::Texture2D::Texture2D(
    const core::Id id,
    const gl::uint txt_obj,
    const render::texture::TextureFormat texture_format,
    engine::Engine* const e) noexcept
    : render::texture::Texture2D(id, texture_format, e)
    , texture_object(txt_obj)
{
}

gearoenix::glc3::texture::Texture2D::~Texture2D() noexcept
{
    if (texture_object == 0)
        return;
    render_engine->get_function_loader()->load([c_texture_object { texture_object }] {
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

void gearoenix::glc3::texture::Texture2D::write_gx3d(
    const std::shared_ptr<system::stream::Stream>&,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::glc3::texture::Texture2D::bind(gl::enumerated texture_unit) const noexcept
{
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    bind();
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
}

void gearoenix::glc3::texture::Texture2D::bind() const noexcept
{
    gl::Loader::bind_texture(GL_TEXTURE_2D, texture_object);
}

gearoenix::gl::sint gearoenix::glc3::texture::Texture2D::convert_internal_format(gearoenix::render::texture::TextureFormat f) noexcept
{
    switch (f) {
    case render::texture::TextureFormat::RgbaFloat32:
        return GL_RGBA32F;
    case render::texture::TextureFormat::RgbFloat32:
        return GL_RGB32F;
    case render::texture::TextureFormat::RgFloat32:
        return GL_RG32F;
    case render::texture::TextureFormat::RgbFloat16:
        return GL_RGB16F;
    case render::texture::TextureFormat::RgbaUint8:
        return GL_RGBA;
    case render::texture::TextureFormat::RgbUint8:
        return GL_RGB;
    case render::texture::TextureFormat::D32:
        return GL_DEPTH_COMPONENT32F;
    default:
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << int(f))
    }
}

gearoenix::gl::enumerated gearoenix::glc3::texture::Texture2D::convert_format(gearoenix::render::texture::TextureFormat f) noexcept
{

    switch (f) {
    case render::texture::TextureFormat::RgbaUint8:
    case render::texture::TextureFormat::RgbaFloat32:
        return GL_RGBA;
    case render::texture::TextureFormat::RgbUint8:
    case render::texture::TextureFormat::RgbFloat32:
    case render::texture::TextureFormat::RgbFloat16:
        return GL_RGB;
    case render::texture::TextureFormat::RgFloat32:
    case render::texture::TextureFormat::RgFloat16:
        return GL_RG;
    case render::texture::TextureFormat::D32:
        return GL_DEPTH_COMPONENT;
    default:
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << int(f))
    }
}

gearoenix::gl::enumerated gearoenix::glc3::texture::Texture2D::convert_data_format(gearoenix::render::texture::TextureFormat f) noexcept
{

    switch (f) {
    case render::texture::TextureFormat::RgbaFloat32:
    case render::texture::TextureFormat::RgbFloat32:
    case render::texture::TextureFormat::RgFloat32:
    case render::texture::TextureFormat::D32:
        return GL_FLOAT;
    case render::texture::TextureFormat::RgbFloat16:
        return GL_HALF_FLOAT;
    case render::texture::TextureFormat::RgbaUint8:
    case render::texture::TextureFormat::RgbUint8:
        return GL_UNSIGNED_BYTE;
    default:
        GXLOGF("Unsupported/Unimplemented setting for texture with id " << int(f))
    }
}

void gearoenix::glc3::texture::Texture2D::generate_mipmap() const noexcept
{
    bind();
    gl::Loader::generate_mipmap(GL_TEXTURE_2D);
}

#endif

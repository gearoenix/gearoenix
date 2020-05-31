#include "glc3-txt-2d.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-sample.hpp"

gearoenix::glc3::texture::Texture2D::Texture2D(
    const core::Id id,
    const render::texture::TextureFormat texture_format,
    const render::texture::SampleInfo& sample_info,
    engine::Engine* const e) noexcept
    : render::texture::Texture2D(id, texture_format, sample_info, e)
{
}

std::shared_ptr<gearoenix::glc3::texture::Texture2D> gearoenix::glc3::texture::Texture2D::construct(
    const core::Id my_id,
    engine::Engine* const e,
    std::vector<std::vector<std::uint8_t>> data,
    const render::texture::TextureInfo& info,
    const std::size_t img_width,
    const std::size_t img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(my_id, info.format, info.sample_info, e));
    result->img_width = img_width;
    result->img_height = img_height;
    const SampleInfo sample_info(info.sample_info);
    const bool needs_mipmap = info.has_mipmap;
    const auto internal_format = convert_internal_format(e, result->texture_format);
    const auto format = convert_format(result->texture_format);
    const auto data_format = convert_data_format(result->texture_format);
    const auto gl_img_width = static_cast<gl::sizei>(img_width);
    const auto gl_img_height = static_cast<gl::sizei>(img_height);
    std::vector<std::vector<std::uint8_t>> pixels;
    if (data.empty() || data[0].empty()) {
        pixels.emplace_back(img_width * img_height * 4 * 4);
        for (auto& p : pixels[0])
            p = 0;
    } else {
        switch (info.format) {
        case render::texture::TextureFormat::RgbaFloat32:
            if (e->get_engine_type() == render::engine::Type::OPENGL_ES3) {
                pixels = convert_float_pixels(data, 4, 4);
            } else {
                pixels = std::move(data);
            }
            break;
        case render::texture::TextureFormat::RgbFloat32:
            if (e->get_engine_type() == render::engine::Type::OPENGL_ES3) {
                pixels = convert_float_pixels(data, 3, 3);
            } else {
                pixels = std::move(data);
            }
            break;
        case render::texture::TextureFormat::RgFloat32:
            if (e->get_engine_type() == render::engine::Type::OPENGL_ES3) {
                pixels = convert_float_pixels(data, 2, 2);
            } else {
                pixels = std::move(data);
            }
            break;
        case render::texture::TextureFormat::RgbaUint8:
        case render::texture::TextureFormat::RgbUint8:
        case render::texture::TextureFormat::RgUint8:
        case render::texture::TextureFormat::Uint8:
            pixels = std::move(data);
            break;
        default:
            GXLOGF("Unsupported/Unimplemented setting for texture with id " << my_id)
        }
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
        for (std::size_t level_index = 0; level_index < pixels.size(); ++level_index) {
            const auto li = static_cast<gl::sint>(level_index);
            const auto lw = static_cast<gl::sizei>(gl_img_width >> level_index);
            const auto lh = static_cast<gl::sizei>(gl_img_height >> level_index);
            gl::Loader::tex_image_2d(GL_TEXTURE_2D, li, internal_format, lw, lh, 0, format, data_format, pixels[level_index].data());
        }
        if (needs_mipmap && pixels.size() < 2) {
            gl::Loader::generate_mipmap(GL_TEXTURE_2D);
        }
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
    });
    return result;
}

gearoenix::glc3::texture::Texture2D::Texture2D(
    const core::Id id,
    const gl::uint txt_obj,
    const render::texture::TextureFormat texture_format,
    engine::Engine* const e) noexcept
    : render::texture::Texture2D(id, texture_format, render::texture::SampleInfo(), e)
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
    GX_UNIMPLEMENTED
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

gearoenix::gl::sint gearoenix::glc3::texture::Texture2D::convert_internal_format(
    engine::Engine* const e, render::texture::TextureFormat& f) noexcept
{
    switch (f) {
    case render::texture::TextureFormat::RgbaFloat32:
        if (e->get_engine_type() == render::engine::Type::OPENGL_ES3) {
            f = render::texture::TextureFormat::RgbaUint8;
            return GL_RGBA;
        }
        return GL_RGBA32F;
    case render::texture::TextureFormat::RgbFloat32:
        if (e->get_engine_type() == render::engine::Type::OPENGL_ES3) {
            f = render::texture::TextureFormat::RgbUint8;
            return GL_RGB;
        }
        return GL_RGB32F;
    case render::texture::TextureFormat::RgFloat32:
        if (e->get_engine_type() == render::engine::Type::OPENGL_ES3) {
            f = render::texture::TextureFormat::RgUint8;
            return GL_RG;
        }
        return GL_RG32F;
    case render::texture::TextureFormat::RgbFloat16:
        if (e->get_engine_type() == render::engine::Type::OPENGL_ES3) {
            f = render::texture::TextureFormat::RgbUint8;
            return GL_RGB;
        }
        return GL_RGB16F;
    case render::texture::TextureFormat::RgbaUint8:
        return GL_RGBA;
    case render::texture::TextureFormat::RgbUint8:
        return GL_RGB;
    case render::texture::TextureFormat::RgUint8:
        return GL_RG;
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
    case render::texture::TextureFormat::RgUint8:
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
    case render::texture::TextureFormat::RgUint8:
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

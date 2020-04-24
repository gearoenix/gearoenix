#include "gles2-txt-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-sample.hpp"
#include <cmath>

gearoenix::gles2::texture::Texture2D::Texture2D(
    const core::Id id,
    const render::texture::TextureFormat texture_format,
    const render::texture::SampleInfo& sample_info,
    engine::Engine* const e) noexcept
    : render::texture::Texture2D(id, texture_format, sample_info, e)
{
}

std::shared_ptr<gearoenix::gles2::texture::Texture2D> gearoenix::gles2::texture::Texture2D::construct(
    const core::Id id,
    engine::Engine* const e,
    std::vector<std::vector<std::uint8_t>> data,
    const render::texture::TextureInfo& info,
    const std::size_t img_width,
    const std::size_t img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(id, info.format, info.sample_info, e));
    result->img_width = img_width;
    result->img_height = img_height;
    const SampleInfo sample_info(info.sample_info);
    const bool needs_mipmap = info.has_mipmap;
    const auto cf = convert(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(img_width);
    const auto gl_img_height = static_cast<gl::sizei>(img_height);
    std::vector<std::vector<std::uint8_t>> pixels;
    if (data.empty() || data[0].empty()) {
        pixels.emplace_back(img_width * img_height * 4);
        for (auto& p : pixels[0])
            p = 0;
    } else {
        switch (info.format) {
        case render::texture::TextureFormat::RgbaFloat32:
            pixels = convert_float_pixels(data, 4, 4);
            break;
        case render::texture::TextureFormat::RgbFloat32:
            pixels = convert_float_pixels(data, 3, 3);
            break;
        case render::texture::TextureFormat::RgFloat32:
            pixels = convert_float_pixels(data, 2, 3);
            break;
        case render::texture::TextureFormat::RgbUint8:
        case render::texture::TextureFormat::RgbaUint8: {
            pixels = std::move(data);
            break;
        }
        default:
            GXLOGF("Unsupported/Unimplemented setting for texture with id " << id)
        }
    }
    e->get_function_loader()->load([needs_mipmap, result, pixels { move(pixels) }, cf, gl_img_width, gl_img_height, sample_info, call] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        for (std::size_t level_index = 0; level_index < pixels.size(); ++level_index) {
            gl::Loader::tex_image_2d(
                GL_TEXTURE_2D, static_cast<gl::sint>(level_index),
                static_cast<gl::sint>(cf), gl_img_width >> level_index,
                gl_img_height >> level_index, 0,
                static_cast<gl::enumerated>(cf), GL_UNSIGNED_BYTE,
                pixels[level_index].data());
        }
        if (needs_mipmap && pixels.size() < 2) {
            gl::Loader::generate_mipmap(GL_TEXTURE_2D);
        }
#ifdef GX_DEBUG_GLES2
        gl::Loader::check_for_error();
#endif
    });
    return result;
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
    case render::texture::TextureFormat::RgbUint8:
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

#include "gx-gl-texture.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-loader.hpp"
#include <vector>

gearoenix::gl::sint gearoenix::gl::convert_internal_format(const render::texture::TextureFormat f) noexcept
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
    case render::texture::TextureFormat::D32:
        return GL_DEPTH_COMPONENT32F;
    default:
        GX_LOG_F("Unsupported or unimplemented setting for texture with id " << static_cast<int>(f));
    }
}

gearoenix::gl::enumerated gearoenix::gl::convert_format(const render::texture::TextureFormat f) noexcept
{
    switch (f) {
    case render::texture::TextureFormat::RgbaUint8:
    case render::texture::TextureFormat::RgbaFloat32:
        return GL_RGBA;
    case render::texture::TextureFormat::RgbFloat32:
    case render::texture::TextureFormat::RgbFloat16:
        return GL_RGB;
    case render::texture::TextureFormat::RgFloat32:
    case render::texture::TextureFormat::RgFloat16:
        return GL_RG;
    case render::texture::TextureFormat::D32:
        return GL_DEPTH_COMPONENT;
    default:
        GX_LOG_F("Unsupported or unimplemented setting for texture with id " << static_cast<int>(f));
    }
}

gearoenix::gl::enumerated gearoenix::gl::convert_data_format(const render::texture::TextureFormat f) noexcept
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
        return GL_UNSIGNED_BYTE;
    default:
        GX_LOG_F("Unsupported or unimplemented setting for texture with id " << static_cast<int>(f));
    }
}

float gearoenix::gl::convert_min(const render::texture::Filter f) noexcept
{
    switch (f) {
    case render::texture::Filter::Nearest:
        return GL_NEAREST;
    case render::texture::Filter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case render::texture::Filter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case render::texture::Filter::Linear:
        return GL_LINEAR;
    case render::texture::Filter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case render::texture::Filter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    default:
        GX_UNEXPECTED;
    }
}

float gearoenix::gl::convert_mag(const render::texture::Filter f) noexcept
{
    switch (f) {
    case render::texture::Filter::Nearest:
        return GL_NEAREST;
    case render::texture::Filter::Linear:
        return GL_LINEAR;
    default:
        GX_UNEXPECTED;
    }
}

gearoenix::gl::sint gearoenix::gl::convert(const render::texture::Wrap w) noexcept
{
    switch (w) {
    case render::texture::Wrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case render::texture::Wrap::Mirror:
        return GL_MIRRORED_REPEAT;
    case render::texture::Wrap::Repeat:
        return GL_REPEAT;
    default:
        GX_UNEXPECTED;
    }
}

gearoenix::gl::Texture2D::Texture2D(
    Engine& e,
    const render::texture::TextureInfo& info,
    std::string name) noexcept
    : render::texture::Texture2D(std::move(name), info, e)
    , e(e)
{
}

gearoenix::gl::Texture2D::~Texture2D() noexcept
{
    e.todos.load([o = object]() {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &o);
    });
}

void gearoenix::gl::Texture2D::bind(const enumerated texture_unit) noexcept
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, object);
}

gearoenix::gl::TextureManager::TextureManager(Engine& e) noexcept
    : e(e)
{
}

gearoenix::gl::TextureManager::~TextureManager() noexcept = default;

[[nodiscard]] std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::gl::TextureManager::create_2d_from_pixels(
    std::string name,
    std::vector<std::vector<std::uint8_t>> pixels,
    const render::texture::TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(e, info, std::move(name)));
    const bool needs_mipmap_generation = info.has_mipmap && pixels.size() < 2;
    const auto internal_format = convert_internal_format(info.format);
    const auto format = convert_format(info.format);
    const auto data_format = convert_data_format(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(info.width);
    const auto gl_img_height = static_cast<gl::sizei>(info.height);
    e.todos.load([result, needs_mipmap_generation, pixels = std::move(pixels), internal_format, format, data_format, gl_img_width, gl_img_height, info, c = c] {
        glGenTextures(1, &(result->object));
        glBindTexture(GL_TEXTURE_2D, result->object);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert_min(info.sampler_info.min_filter));
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert_mag(info.sampler_info.mag_filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert(info.sampler_info.wrap_s));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert(info.sampler_info.wrap_t));
        if (pixels.size() > 1 || !info.has_mipmap) {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<float>(pixels.size() - 1));
        }
        for (std::size_t level_index = 0; level_index < pixels.size(); ++level_index) {
            const auto li = static_cast<gl::sint>(level_index);
            auto lw = static_cast<gl::sizei>(gl_img_width >> level_index);
            if (lw < 1)
                lw = 1;
            auto lh = static_cast<gl::sizei>(gl_img_height >> level_index);
            if (lh < 1)
                lh = 1;
            glTexImage2D(GL_TEXTURE_2D, li, internal_format, lw, lh, 0, format, data_format, pixels[level_index].data());
        }
        if (needs_mipmap_generation) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        GX_GL_CHECK_D;
    });
    return result;
}

#endif

#include "gx-gl-texture.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../platform/stream/gx-plt-stm-local.hpp"
#include "../render/texture/gx-rnd-txt-image.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-loader.hpp"
#include "gx-gl-target.hpp"
#include <vector>

#ifdef GX_DEBUG_MODE
// #define GX_DEBUG_TEXTURE_WRITE
#endif

gearoenix::gl::sint gearoenix::gl::convert_internal_format(const render::texture::TextureFormat f) noexcept
{
    switch (f) {
    case render::texture::TextureFormat::Float16:
        return GL_R16F;
    case render::texture::TextureFormat::Float32:
        return GL_R32F;
    case render::texture::TextureFormat::RgbaFloat16:
        return GL_RGBA16F;
    case render::texture::TextureFormat::RgbaFloat32:
        return GL_RGBA32F;
    case render::texture::TextureFormat::RgbaUint8:
        return GL_RGBA;
    case render::texture::TextureFormat::RgbFloat16:
        return GL_RGB16F;
    case render::texture::TextureFormat::RgbFloat32:
        return GL_RGB32F;
    case render::texture::TextureFormat::RgFloat16:
        return GL_RG16F;
    case render::texture::TextureFormat::RgFloat32:
        return GL_RG32F;
    case render::texture::TextureFormat::D32:
        return GL_DEPTH_COMPONENT32F;
    default:
        GX_LOG_F("Unsupported or unimplemented setting for texture with id " << static_cast<int>(f));
    }
}

gearoenix::gl::enumerated gearoenix::gl::convert_format(const render::texture::TextureFormat f) noexcept
{
    switch (f) {
    case render::texture::TextureFormat::Float16:
    case render::texture::TextureFormat::Float32:
        return GL_RED;
    case render::texture::TextureFormat::RgbaFloat16:
    case render::texture::TextureFormat::RgbaFloat32:
    case render::texture::TextureFormat::RgbaUint8:
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
    case render::texture::TextureFormat::D32:
    case render::texture::TextureFormat::Float32:
    case render::texture::TextureFormat::RgbaFloat32:
    case render::texture::TextureFormat::RgbFloat32:
    case render::texture::TextureFormat::RgFloat32:
        return GL_FLOAT;
    case render::texture::TextureFormat::Float16:
    case render::texture::TextureFormat::RgbaFloat16:
    case render::texture::TextureFormat::RgbFloat16:
    case render::texture::TextureFormat::RgFloat16:
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

gearoenix::gl::enumerated gearoenix::gl::convert(render::texture::Face f) noexcept
{
    switch (f) {
    case render::texture::Face::PositiveX:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case render::texture::Face::NegativeX:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case render::texture::Face::PositiveY:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case render::texture::Face::NegativeY:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case render::texture::Face::PositiveZ:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case render::texture::Face::NegativeZ:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    default:
        GX_UNEXPECTED;
    }
}

void gearoenix::gl::Texture2D::write(const std::shared_ptr<platform::stream::Stream>& s, const core::sync::EndCallerIgnored& c) const noexcept
{
    render::texture::Texture2D::write(s, c);
    e.todos.load([this, s, c] {
        GX_GL_CHECK_D;
        gl::uint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glBindTexture(GL_TEXTURE_2D, object);
        std::vector<std::uint8_t> data(info.width * info.height * format_pixel_size(info.format));
        auto level_width = static_cast<gl::sizei>(info.width);
        auto level_height = static_cast<gl::sizei>(info.height);
        const auto mips_count = get_mipmaps_count();
        for (gl::sint mip_index = 0; mip_index < mips_count; ++mip_index, level_width >>= 1u, level_height >>= 1u) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, object, mip_index);
            glReadPixels(0, 0, level_width, level_height, convert_format(info.format), convert_data_format(info.format), data.data());
#ifdef GX_DEBUG_TEXTURE_WRITE
            const auto ext = render::texture::format_has_float_component(info.format) ? "hdr" : "png";
            platform::stream::Local l(e.get_platform_application(),
                "texture-2d-gl-name-" + name + "-level-" + std::to_string(mip_index) + "." + ext, true);
            write_image(l, data.data(), level_width, level_height, info.format);
#endif
            write_gx3d_image(*s, data.data(), level_width, level_height, info.format);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &framebuffer);
        GX_GL_CHECK_D;
    });
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

void gearoenix::gl::Texture2D::generate_mipmaps() noexcept
{
    glBindTexture(GL_TEXTURE_2D, object);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void gearoenix::gl::TextureCube::write(const std::shared_ptr<platform::stream::Stream>& s, const core::sync::EndCallerIgnored& c) const noexcept
{
    render::texture::TextureCube::write(s, c);
    e.todos.load([this, s, c] {
        GX_GL_CHECK_D;
        gl::uint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glBindTexture(GL_TEXTURE_CUBE_MAP, object);
        std::vector<std::uint8_t> data(info.width * info.width * format_pixel_size(info.format));
        for (auto face : render::texture::FACES) {
            auto level_aspect = static_cast<gl::sizei>(info.width);
            const auto mips_count = get_mipmaps_count();
            for (gl::sint mipmap_index = 0; mipmap_index < mips_count; ++mipmap_index, level_aspect >>= 1u) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, convert(face), object, mipmap_index);
                glReadPixels(0, 0, level_aspect, level_aspect, convert_format(info.format), convert_data_format(info.format), data.data());
#ifdef GX_DEBUG_TEXTURE_WRITE
                const auto ext = render::texture::format_has_float_component(info.format) ? "hdr" : "png";
                platform::stream::Local l(e.get_platform_application(),
                    "texture-cube-gl-name-" + name + "-face-" + std::to_string(face) + "-level-" + std::to_string(mipmap_index) + "." + ext, true);
                write_image(l, data.data(), level_aspect, level_aspect, info.format);
#endif
                write_gx3d_image(*s, data.data(), level_aspect, level_aspect, info.format);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &framebuffer);
        GX_GL_CHECK_D;
    });
}

gearoenix::gl::TextureCube::TextureCube(
    Engine& e,
    const render::texture::TextureInfo& info,
    std::string name) noexcept
    : render::texture::TextureCube(std::move(name), info, e)
    , e(e)
{
}

gearoenix::gl::TextureCube::~TextureCube() noexcept
{
    e.todos.load([o = object]() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures(1, &o);
    });
}

void gearoenix::gl::TextureCube::bind(const enumerated texture_unit) noexcept
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, object);
}

gearoenix::gl::TextureManager::TextureManager(Engine& e) noexcept
    : render::texture::Manager(e)
    , eng(e)
{
}

gearoenix::gl::TextureManager::~TextureManager() noexcept = default;

[[nodiscard]] std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::gl::TextureManager::create_2d_from_pixels_v(
    std::string name,
    std::vector<std::vector<std::uint8_t>> pixels,
    const render::texture::TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    std::shared_ptr<Texture2D> result(new Texture2D(eng, info, std::move(name)));
    const bool needs_mipmap_generation = info.has_mipmap && pixels.size() < 2;
    const auto internal_format = convert_internal_format(info.format);
    const auto format = convert_format(info.format);
    const auto data_format = convert_data_format(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(info.width);
    const auto gl_img_height = static_cast<gl::sizei>(info.height);
    eng.todos.load([result, needs_mipmap_generation, pixels = std::move(pixels), internal_format, format, data_format, gl_img_width, gl_img_height, info, c = c] {
        GX_GL_CHECK_D;
        glGenTextures(1, &(result->object));
        glBindTexture(GL_TEXTURE_2D, result->object);
        GX_GL_CHECK_D;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert_min(info.sampler_info.min_filter));
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert_mag(info.sampler_info.mag_filter));
        GX_GL_CHECK_D;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert(info.sampler_info.wrap_s));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert(info.sampler_info.wrap_t));
        GX_GL_CHECK_D;
        if (pixels.size() > 1 || (!pixels.empty() && !info.has_mipmap)) {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<float>(pixels.size() - 1));
        }
        if (GL_DEPTH_COMPONENT32F == internal_format) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        }
        GX_GL_CHECK_D;
        if (pixels.empty()) {
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, gl_img_width, gl_img_height, 0, format, data_format, nullptr);
        } else {
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
        }
        if (needs_mipmap_generation) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        GX_GL_CHECK_D;
    });
    return result;
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::gl::TextureManager::create_cube_from_pixels_v(
    std::string name,
    std::vector<std::vector<std::vector<std::uint8_t>>> pixels,
    const render::texture::TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    std::shared_ptr<TextureCube> result(new TextureCube(eng, info, std::move(name)));
    const bool needs_mipmap_generation = info.has_mipmap && (pixels.empty() || pixels[0].size() < 2);
    const auto internal_format = convert_internal_format(info.format);
    const auto format = convert_format(info.format);
    const auto data_format = convert_data_format(info.format);
    const auto gl_img_width = static_cast<gl::sizei>(info.width);
    const auto gl_img_height = static_cast<gl::sizei>(info.height);
    eng.todos.load([result, needs_mipmap_generation, pixels = std::move(pixels), internal_format, format, data_format, gl_img_width, gl_img_height, info, c = c] {
        GX_GL_CHECK_D;
        glGenTextures(1, &(result->object));
        glBindTexture(GL_TEXTURE_CUBE_MAP, result->object);
        GX_GL_CHECK_D;
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, convert_min(info.sampler_info.min_filter));
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, convert_mag(info.sampler_info.mag_filter));
        GX_GL_CHECK_D;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, convert(info.sampler_info.wrap_s));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, convert(info.sampler_info.wrap_t));
        GX_GL_CHECK_D;
        if (pixels.size() > 1 || (!pixels.empty() && !info.has_mipmap)) {
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, static_cast<float>(pixels.size() - 1));
        }
        if (GL_DEPTH_COMPONENT32F == internal_format) {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        }
        GX_GL_CHECK_D;
        for (std::size_t face_index = 0; face_index < 6; ++face_index) {
            const auto gl_face = convert(render::texture::FACES[face_index]);
            if (pixels.empty() || pixels[face_index].empty()) {
                glTexImage2D(gl_face, 0, internal_format, gl_img_width, gl_img_height, 0, format, data_format, nullptr);
            } else {
                const auto& face_pixels = pixels[face_index];
                for (std::size_t level_index = 0; level_index < face_pixels.size(); ++level_index) {
                    const auto li = static_cast<gl::sint>(level_index);
                    auto lw = static_cast<gl::sizei>(gl_img_width >> level_index);
                    if (lw < 1)
                        lw = 1;
                    auto lh = static_cast<gl::sizei>(gl_img_height >> level_index);
                    if (lh < 1)
                        lh = 1;
                    glTexImage2D(gl_face, li, internal_format, lw, lh, 0, format, data_format, face_pixels[level_index].data());
                }
            }
        }
        if (needs_mipmap_generation) {
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        GX_GL_CHECK_D;
    });
    return result;
}

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::gl::TextureManager::create_target_v(
    std::string name,
    std::vector<render::texture::Attachment>&& attachments,
    const core::sync::EndCallerIgnored& c) noexcept
{
    return std::make_shared<Target>(eng, std::move(attachments), c);
}

#endif

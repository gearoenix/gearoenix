#include "gx-gles2-txt-cube.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/gx-cr-function-loader.hpp"
#include "../../gl/gx-gl-constants.hpp"
#include "../../gl/gx-gl-loader.hpp"
#include "../../system/stream/gx-sys-stm-stream.hpp"
#include "../engine/gx-gles2-eng-engine.hpp"
#include "gx-gles2-txt-2d.hpp"
#include "gx-gles2-txt-sample.hpp"
#include <cmath>

static const gearoenix::gl::enumerated FACES[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
};

gearoenix::gles2::texture::TextureCube::TextureCube(
    const core::Id id,
    std::string name,
    const render::texture::TextureFormat texture_format,
    const render::texture::SamplerInfo& sampler_info,
    engine::Engine* const engine) noexcept
    : render::texture::TextureCube(id, std::move(name), texture_format, sampler_info, engine)
{
}

std::shared_ptr<gearoenix::gles2::texture::TextureCube> gearoenix::gles2::texture::TextureCube::construct(
    const core::Id id,
    std::string name,
    engine::Engine* const e,
    std::vector<std::vector<std::vector<std::uint8_t>>> data,
    const render::texture::TextureInfo& info,
    const unsigned int aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    std::shared_ptr<TextureCube> result(new TextureCube(id, std::move(name), info.format, info.sampler_info, e));
    result->aspect = aspect;
    const SamplerInfo sample_info = SamplerInfo(info.sampler_info);
    const auto cf = Texture2D::convert(info.format);
    const auto gl_aspect = static_cast<gl::sizei>(aspect);
    std::vector<std::vector<std::vector<std::uint8_t>>> pixels(GX_COUNT_OF(FACES));
    if (data.empty() || data[0].empty() || data[0][0].empty()) {
        std::vector<std::vector<std::uint8_t>> black(1);
        black[0].resize(aspect * aspect * 16);
        for (auto& p : black[0])
            p = 0;
        pixels[0] = black;
        pixels[1] = black;
        pixels[2] = black;
        pixels[3] = black;
        pixels[4] = black;
        pixels[5] = std::move(black);
    } else {
        switch (info.format) {
        case render::texture::TextureFormat::RgbaFloat32:
            pixels = convert_float_pixels(data, 4, 4);
            break;
        case render::texture::TextureFormat::RgbFloat32:
            pixels = convert_float_pixels(data, 3, 3);
            break;
        case render::texture::TextureFormat::RgbaUint8:
        case render::texture::TextureFormat::RgbUint8:
            pixels = std::move(data);
            break;
        default:
            GXLOGF("Unsupported/Unimplemented setting for cube texture with id " << id)
        }
    }
    e->get_function_loader()->load([result, gl_aspect, pixels { move(pixels) }, cf, sample_info, needs_mipmap { info.has_mipmap }, call] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        for (int fi = 0; fi < static_cast<int>(GX_COUNT_OF(FACES)); ++fi) {
            const auto& face_pixels = pixels[fi];
            for (std::size_t level_index = 0; level_index < face_pixels.size(); ++level_index) {
                gl::Loader::tex_image_2d(
                    FACES[fi],
                    static_cast<gl::sint>(level_index),
                    static_cast<gl::sint>(cf),
                    static_cast<gl::sizei>(gl_aspect >> level_index),
                    static_cast<gl::sizei>(gl_aspect >> level_index),
                    0, cf, GL_UNSIGNED_BYTE, face_pixels[level_index].data());
            }
        }
#ifdef GX_DEBUG_GLES2
        gl::Loader::check_for_error();
#endif
        if (needs_mipmap && pixels[0].size() < 2) {
            gl::Loader::generate_mipmap(GL_TEXTURE_CUBE_MAP);
            // It clears the errors, some drivers does not support mip-map generation for cube texture
            gl::Loader::get_error();
        }
    });
    return result;
}

gearoenix::gles2::texture::TextureCube::~TextureCube() noexcept
{
    if (texture_object == 0)
        return;
    const gl::uint c_texture_object = texture_object;
    render_engine->get_function_loader()->load([c_texture_object] {
        gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, 0);
        gl::Loader::delete_textures(1, &c_texture_object);
    });
    texture_object = 0;
}

void gearoenix::gles2::texture::TextureCube::write_gx3d(
    const std::shared_ptr<system::stream::Stream>&,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>&) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::gles2::texture::TextureCube::bind(gl::enumerated texture_unit) const noexcept
{
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    bind();
}

void gearoenix::gles2::texture::TextureCube::bind() const noexcept
{
    gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, texture_object);
}

#endif

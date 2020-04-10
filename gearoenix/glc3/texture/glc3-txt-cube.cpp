#include "glc3-txt-cube.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-image.hpp"
#include "../../system/stream/sys-stm-local.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-2d.hpp"
#include "glc3-txt-sample.hpp"

static const gearoenix::gl::enumerated FACES[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
};

gearoenix::glc3::texture::TextureCube::TextureCube(
    const core::Id id,
    const render::texture::TextureFormat texture_format,
    engine::Engine* const engine) noexcept
    : render::texture::TextureCube(id, texture_format, engine)
{
}

std::shared_ptr<gearoenix::glc3::texture::TextureCube> gearoenix::glc3::texture::TextureCube::construct(
    const core::Id id,
    engine::Engine* const engine,
    const void* const data,
    const render::texture::TextureInfo& info,
    const unsigned int aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<TextureCube> result(new TextureCube(id, info.format, engine));
    result->aspect = aspect;
    const SampleInfo sample_info = SampleInfo(info.sample_info);
    const auto internal_format = Texture2D::convert_internal_format(info.format);
    const auto format = Texture2D::convert_format(info.format);
    const auto data_format = Texture2D::convert_data_format(info.format);
    const auto gl_aspect = static_cast<gl::sizei>(aspect);
    std::vector<std::vector<std::uint8_t>> pixels(GX_COUNT_OF(FACES));
    if (nullptr != data) {
        switch (info.format) {
        case render::texture::TextureFormat::RgbaFloat32: {
            const auto pixel_size = gl_aspect * gl_aspect * 4 * 4;
            auto src = reinterpret_cast<std::size_t>(data);
            for (int fi = 0; fi < static_cast<int>(GX_COUNT_OF(FACES)); ++fi, src += pixel_size) {
                auto& face_pixels = pixels[fi];
                face_pixels.resize(pixel_size);
                std::memcpy(face_pixels.data(), reinterpret_cast<const void*>(src), pixel_size);
            }
            break;
        }
        default:
            GXLOGF("Unsupported/Unimplemented setting for cube texture with id " << id)
        }
    }
    engine->get_function_loader()->load([result, gl_aspect, pixels { move(pixels) }, internal_format, format, data_format, sample_info, call] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        for (int fi = 0; fi < static_cast<int>(GX_COUNT_OF(FACES)); ++fi) {
            const auto& face_pixels = pixels[fi];
            gl::Loader::tex_image_2d(FACES[fi], 0, internal_format, gl_aspect, gl_aspect, 0, format, data_format,
                face_pixels.empty() ? nullptr : pixels[fi].data());
        }
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        gl::Loader::generate_mipmap(GL_TEXTURE_CUBE_MAP);
        // It clears the errors, some drivers does not support mip-map generation for cube texture
        gl::Loader::get_error();
    });
    return result;
}

std::shared_ptr<gearoenix::glc3::texture::TextureCube> gearoenix::glc3::texture::TextureCube::construct(
    const core::Id id,
    engine::Engine* const engine,
    const render::texture::TextureInfo& info,
    const unsigned int aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<TextureCube> result(new TextureCube(id, info.format, engine));
    const SampleInfo sample_info = SampleInfo(info.sample_info);
    gl::enumerated format, data_format;
    gl::uint internal_format;
    const auto gl_aspect = static_cast<gl::sizei>(aspect);
    switch (info.format) {
    case render::texture::TextureFormat::RgbaFloat32:
        internal_format = GL_RGBA32F;
        format = GL_RGBA;
        data_format = GL_FLOAT;
        break;
    default:
        GXUNEXPECTED
    }
    const auto zero_data_size = aspect * aspect * 4 * 4;
    auto* const zero_data = new unsigned char[zero_data_size];
    std::memset(zero_data, 0, zero_data_size);
    engine->get_function_loader()->load([result, gl_aspect, internal_format, format, data_format, sample_info, has_mipmap { info.has_mipmap }, call, zero_data] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        for (unsigned int f : FACES) {
            gl::Loader::tex_image_2d(f, 0, internal_format, gl_aspect, gl_aspect, 0, format, data_format, zero_data);
        }
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        if (has_mipmap)
            gl::Loader::generate_mipmap(GL_TEXTURE_CUBE_MAP);
        // It clears the errors, some drivers does not support mip-map generation for cube texture
        gl::Loader::get_error();
        delete[] zero_data;
    });
    return result;
}

gearoenix::glc3::texture::TextureCube::~TextureCube() noexcept
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

void gearoenix::glc3::texture::TextureCube::write_gx3d(
    const std::shared_ptr<system::stream::Stream>& s,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& end_call) noexcept
{
    render::texture::TextureCube::write_gx3d(s, end_call);
    render_engine->get_function_loader()->load([this, s, end_call] {
        gl::uint framebuffer;
        gl::Loader::gen_framebuffers(1, &framebuffer);
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
        bind();
        if (render::texture::format_has_float_component(texture_format)) {
            std::vector<float> data(aspect * aspect * 4);
            for (int i = 0; i < 6; ++i) {
                for (unsigned int j = 0, level_aspect = aspect; level_aspect > 0; ++j, level_aspect >>= 1u) {
                    gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FACES[i], texture_object,
                        j);
                    gl::Loader::read_pixels(0, 0, level_aspect, level_aspect, GL_RGBA, GL_FLOAT, data.data());
#ifdef GX_DEBUG_MODE
                    system::stream::Local l(
                        "texture-cube-glc3-id" + std::to_string(asset_id) + "-face" + std::to_string(i) + "-level" + std::to_string(j) + ".hdr", true);
                    render::texture::Image::encode_hdr(&l, data.data(), level_aspect, level_aspect, 4);
#endif
                    write_gx3d_image(s.get(), data.data(), level_aspect, level_aspect, 4);
                }
            }
        } else {
            std::vector<unsigned char> data(aspect * aspect * 4);
            for (int i = 0; i < 6; ++i) {
                for (unsigned int j = 0, level_aspect = aspect; level_aspect > 0; ++j, level_aspect >>= 1u) {
                    gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FACES[i], texture_object,
                        j);
                    gl::Loader::read_pixels(0, 0, level_aspect, level_aspect, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
#ifdef GX_DEBUG_MODE
                    system::stream::Local l(
                        "texture-cube-glc3-id" + std::to_string(asset_id) + "-face" + std::to_string(i) + "-level" + std::to_string(j) + ".png", true);
                    render::texture::Image::encode_png(&l, data.data(), level_aspect, level_aspect, 4);
#endif
                    write_gx3d_image(s.get(), data.data(), level_aspect, level_aspect, 4);
                }
            }
        }
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
        gl::Loader::delete_framebuffers(1, &framebuffer);
    });
}

void gearoenix::glc3::texture::TextureCube::bind(gl::enumerated texture_unit) const noexcept
{
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    bind();
}

void gearoenix::glc3::texture::TextureCube::bind() const noexcept
{
    gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, texture_object);
}

void gearoenix::glc3::texture::TextureCube::generate_mipmap() const noexcept
{
    bind();
    gl::Loader::generate_mipmap(GL_TEXTURE_CUBE_MAP);
}

#endif

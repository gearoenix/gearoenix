#include "glc3-txt-cube.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-image.hpp"
#include "../../system/stream/sys-stm-local.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-2d.hpp"
#include "glc3-txt-sample.hpp"

#ifdef GX_DEBUG_MODE
//#define GX_DEBUG_TEXTURE_WRITE
#endif

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
    const render::texture::SampleInfo& sample_info,
    engine::Engine* const engine) noexcept
    : render::texture::TextureCube(id, texture_format, sample_info, engine)
{
}

std::shared_ptr<gearoenix::glc3::texture::TextureCube> gearoenix::glc3::texture::TextureCube::construct(
    const core::Id id,
    engine::Engine* const engine,
    std::vector<std::vector<std::vector<std::uint8_t>>> data,
    const render::texture::TextureInfo& info,
    const std::size_t aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<TextureCube> result(new TextureCube(id, info.format, info.sample_info, engine));
    result->aspect = aspect;
    const SampleInfo sample_info = SampleInfo(info.sample_info);
    const auto internal_format = Texture2D::convert_internal_format(info.format);
    const auto format = Texture2D::convert_format(info.format);
    const auto data_format = Texture2D::convert_data_format(info.format);
    const auto gl_aspect = static_cast<gl::sizei>(aspect);
    std::vector<std::vector<std::vector<std::uint8_t>>> pixels;
    if (data.empty() || data[0].empty() || data[0][0].empty()) {
        pixels.resize(6);
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
        case render::texture::TextureFormat::RgbaFloat32: {
            /// TODO query device for checking the support of the format
            pixels = std::move(data);
            break;
        }
        default:
            GXLOGF("Unsupported/Unimplemented setting for cube texture with id " << id)
        }
    }
    engine->get_function_loader()->load([result, needs_mipmap { info.has_mipmap }, gl_aspect, pixels { move(pixels) }, internal_format, format, data_format, sample_info, call] {
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
                    FACES[fi], level_index, internal_format, gl_aspect >> level_index, gl_aspect >> level_index, 0,
                    format, data_format, face_pixels[level_index].data());
            }
        }
#ifdef GX_DEBUG_GL_CLASS_3
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
        (void)s->write(true); // It means, texture has mipmap data
        if (render::texture::format_has_float_component(texture_format)) {
            std::vector<float> data(aspect * aspect * 4);
            for (int i = 0; i < 6; ++i) {
                for (unsigned int j = 0, level_aspect = aspect; level_aspect > 0; ++j, level_aspect >>= 1u) {
                    gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FACES[i], texture_object,
                        j);
                    gl::Loader::read_pixels(0, 0, level_aspect, level_aspect, GL_RGBA, GL_FLOAT, data.data());
#ifdef GX_DEBUG_TEXTURE_WRITE
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
#ifdef GX_DEBUG_TEXTURE_WRITE
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

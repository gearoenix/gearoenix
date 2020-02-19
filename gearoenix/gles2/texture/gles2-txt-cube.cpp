#include "gles2-txt-cube.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-2d.hpp"
#include "gles2-txt-sample.hpp"

static const gearoenix::gl::enumerated FACES[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
};

gearoenix::gles2::texture::TextureCube::TextureCube(const core::Id my_id, engine::Engine* const engine) noexcept
    : render::texture::TextureCube(my_id, engine)
{
}

std::shared_ptr<gearoenix::gles2::texture::TextureCube> gearoenix::gles2::texture::TextureCube::construct(
    const core::Id my_id,
    engine::Engine* const e,
    const void* const data,
    const render::texture::TextureFormat format,
    const render::texture::SampleInfo s,
    const unsigned int aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<TextureCube> result(new TextureCube(my_id, e));
    const SampleInfo sample_info = SampleInfo(s);
    gl::uint cf;
    const auto gl_aspect = static_cast<gl::sizei>(aspect);
    std::vector<std::vector<std::uint8_t>> pixels(GX_COUNT_OF(FACES));
    if (format == render::texture::TextureFormat::RgbaFloat32 && aspect == 1) {
        cf = GL_RGBA;
        const gl::sizei pixel_size = gl_aspect * gl_aspect * 4;
        const auto* const raw_data = reinterpret_cast<const core::Real*>(data);
        std::uint8_t p[4];
        p[0] = static_cast<std::uint8_t>(raw_data[0] * 255.1f);
        p[1] = static_cast<std::uint8_t>(raw_data[1] * 255.1f);
        p[2] = static_cast<std::uint8_t>(raw_data[2] * 255.1f);
        p[3] = static_cast<std::uint8_t>(raw_data[3] * 255.1f);
        for (int fi = 0; fi < static_cast<int>(GX_COUNT_OF(FACES)); ++fi) {
            pixels[fi].resize(pixel_size);
            for (gl::sizei i = 0; i < pixel_size;)
                for (int j = 0; j < 4; ++j, ++i)
                    pixels[fi][i] = p[j];
        }
    } else
        GXLOGF("Unsupported/Unimplemented setting for cube texture with id " << my_id)
    e->get_function_loader()->load([result, gl_aspect, pixels { move(pixels) }, cf, sample_info, call] {
        gl::Loader::gen_textures(1, &(result->texture_object));
        gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, result->texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
        for (int fi = 0; fi < static_cast<int>(GX_COUNT_OF(FACES)); ++fi) {
            gl::Loader::tex_image_2d(FACES[fi], 0, static_cast<gl::sint>(cf), gl_aspect, gl_aspect, 0, cf, GL_UNSIGNED_BYTE, pixels[fi].data());
        }
        gl::Loader::check_for_error();
        gl::Loader::generate_mipmap(GL_TEXTURE_CUBE_MAP);
        // It clears the errors, some drivers does not support mip-map generation for cube texture
        gl::Loader::get_error();
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

void gearoenix::gles2::texture::TextureCube::bind(gl::enumerated texture_unit) const noexcept
{
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, texture_object);
}

#endif

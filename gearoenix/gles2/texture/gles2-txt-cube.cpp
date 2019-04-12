#include "gles2-txt-cube.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-sample.hpp"
#include "gles2-txt-2d.hpp"

#define GX_FACES_COUNT 6

static const gearoenix::gl::enumerated FACES[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
};

gearoenix::gles2::texture::Cube::Cube(
	const core::Id my_id,
	const std::shared_ptr<engine::Engine>& engine,
	const void *data,
	const render::texture::TextureFormat::Id f,
	const render::texture::SampleInfo s,
	const unsigned int aspect,
	const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
    : render::texture::Cube(my_id, engine)
{
    std::vector<const void *> img_data(GX_FACES_COUNT);
    for (unsigned int i = 0; i < GX_FACES_COUNT; ++i) {
		img_data[i] = reinterpret_cast<const void *>(static_cast<std::size_t>(i * aspect * aspect) + reinterpret_cast<const std::size_t>(data));
    }
	const SampleInfo sample_info = SampleInfo(s);
	const gl::uint cf = Texture2D::convert_format(f);
	engine->get_function_loader()->load([this, aspect, img_data, cf, sample_info, call] {
		gl::Loader::gen_textures(1, &texture_object);
		gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, texture_object);
		gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sample_info.mag_filter);
		gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sample_info.min_filter);
		gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, sample_info.wrap_s);
		gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, sample_info.wrap_t);
		for (int i = 0; i < GX_FACES_COUNT; ++i) {
			gl::Loader::tex_image_2d(FACES[i], 0, static_cast<const gl::sint>(cf), aspect, aspect, 0, cf, GL_UNSIGNED_BYTE, img_data[i]);
		}
		gl::Loader::generate_mipmap(GL_TEXTURE_CUBE_MAP);
		// It clears the errors, some drivers does not support mip-map generation for cube texture
		gl::Loader::get_error();
	});
}

gearoenix::gles2::texture::Cube::~Cube()
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

void gearoenix::gles2::texture::Cube::bind(gl::enumerated texture_unit) const
{
    gl::Loader::active_texture(texture_unit);
    gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, texture_object);
}

#endif

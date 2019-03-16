#include "gles2-txt-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "../../core/cr-function-loader.hpp"

gearoenix::gles2::texture::Texture2D::Texture2D(
	const core::Id my_id,
	const std::shared_ptr<engine::Engine>& e,
	const void * data,
	const render::texture::Format::Id f,
	const render::texture::Sample::Id s,
	const unsigned int img_width,
	const unsigned int img_heigt,
	const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
    : render::texture::Texture2D(my_id, e)
{
	GLfloat min_filter;
	GLfloat mag_filter;
	switch (s)
	{
	case render::texture::Sample::NEAREST:
		min_filter = GL_NEAREST;
		mag_filter = GL_NEAREST;
		break;
	case render::texture::Sample::LINEAR:
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
		mag_filter = GL_LINEAR;
		break;
	default:
		GXUNEXPECTED;
		break;
	}
	GLint internal_format;
	GLenum format;
	GLenum pixel_type;
	switch (f)
	{
	case render::texture::Format::RGBA_UINT8:
		internal_format = GL_RGBA;
		format = GL_RGBA;
		pixel_type = GL_UNSIGNED_BYTE;
		break;
	default:
		GXUNEXPECTED;
		break;
	}
	e->get_function_loader()->load([this, data, f, s, img_width, img_heigt, min_filter, mag_filter, internal_format, format, pixel_type, call]
	{
        glGenTextures(1, &texture_object);
        glBindTexture(GL_TEXTURE_2D, texture_object);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img_width, img_heigt, 0, format, pixel_type, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    });
}

gearoenix::gles2::texture::Texture2D::Texture2D(const core::Id my_id, const GLuint txtobj, const std::shared_ptr<engine::Engine>& e)
    : render::texture::Texture2D(my_id, e)
    , texture_object(txtobj)
{
}

gearoenix::gles2::texture::Texture2D::~Texture2D()
{
	GXUNIMPLEMENTED;
    if (texture_object == 0)
        return;
    const GLuint c_texture_object = texture_object;
//    render_engine->add_load_function([c_texture_object] {
//        glBindTexture(GL_TEXTURE_2D, 0);
//        glDeleteTextures(1, &c_texture_object);
//    });
    texture_object = 0;
}

void gearoenix::gles2::texture::Texture2D::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_object);
}

#endif

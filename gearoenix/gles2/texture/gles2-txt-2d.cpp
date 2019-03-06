#include "gles2-txt-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::gles2::texture::Texture2D::Texture2D(
	const core::Id my_id,
	const std::shared_ptr<system::stream::Stream> &file,
	const std::shared_ptr<engine::Engine> &e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : render::texture::Texture2D(my_id, e)
{
    std::vector<unsigned char> img_data;
    unsigned int imgw, imgh;
    render::texture::PNG::decode(file, img_data, imgw, imgh);
    std::function<void()> loadf = [this, imgw, imgh, img_data, end] {
        glGenTextures(1, &texture_object);
        glBindTexture(GL_TEXTURE_2D, texture_object);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgw, imgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)img_data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        (void)end;
    };
    e->add_load_function(loadf);
}

gearoenix::gles2::texture::Texture2D::Texture2D(const core::Id my_id, const GLuint txtobj, const std::shared_ptr<engine::Engine> &e)
    : render::texture::Texture2D(my_id, e)
    , texture_object(txtobj)
{
}

gearoenix::gles2::texture::Texture2D::~Texture2D()
{
    if (texture_object == 0)
        return;
    const GLuint c_texture_object = texture_object;
    render_engine->add_load_function([c_texture_object] {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &c_texture_object);
    });
    texture_object = 0;
}

void gearoenix::gles2::texture::Texture2D::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_object);
}

#endif

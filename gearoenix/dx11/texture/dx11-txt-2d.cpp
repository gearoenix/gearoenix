#include "gles2-txt-2d.hpp"
#ifdef USE_DIRECTX11
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"

gearoenix::gles2::texture::Texture2D::Texture2D(system::File* file, Engine* eng, std::shared_ptr<core::EndCaller> end)
{
    std::vector<unsigned char> img_data;
    unsigned int imgw, imgh;
    render::texture::PNG::decode(file, img_data, imgw, imgh);
    std::function<void()> loadf = [this, imgw, imgh, img_data, end] {
        glGenTextures(1, &texture_object);
        glBindTexture(GL_TEXTURE_2D, texture_object);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgw, imgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)img_data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        (void)end;
    };
    eng->add_load_function(loadf);
}

gearoenix::gles2::texture::Texture2D::Texture2D(GLuint txtobj)
    : texture_object(txtobj)
{
}

gearoenix::gles2::texture::Texture2D::~Texture2D()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture_object);
}

void gearoenix::gles2::texture::Texture2D::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_object);
}

#endif

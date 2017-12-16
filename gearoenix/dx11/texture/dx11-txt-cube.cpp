#include "gles2-txt-cube.hpp"
#ifdef USE_DIRECTX11
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"

#define FACES_COUNT 6

gearoenix::gles2::texture::Cube::Cube(system::File* file, Engine* eng, std::shared_ptr<core::EndCaller> end)
{
    std::vector<std::vector<unsigned char>> img_data(FACES_COUNT);
    std::vector<unsigned int> imgw(FACES_COUNT), imgh(FACES_COUNT);
    std::vector<core::Offset> img_offs(5);
    for (int i = 0; i < 5; ++i) {
        file->read(img_offs[i]);
    }
    render::texture::PNG::decode(file, img_data[0], imgw[0], imgh[0]);
    for (int i = 1; i < FACES_COUNT; ++i) {
        file->seek((unsigned int)img_offs[i - 1]);
        render::texture::PNG::decode(file, img_data[i], imgw[i], imgh[i]);
    }
    static const GLenum faces[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    };
    std::function<void()> loadf = [&, imgw, imgh, img_data, end]() -> void {
        glGenTextures(1, &texture_object);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_object);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_MIRRORED_REPEAT
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        for (int i = 0; i < FACES_COUNT; ++i) {
            glTexImage2D(faces[i], 0, GL_RGBA, imgw[i], imgh[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)img_data[i].data());
        }
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        (void)end;
        return;
    };
    eng->add_load_function(loadf);
}

gearoenix::gles2::texture::Cube::~Cube()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDeleteTextures(1, &texture_object);
}

void gearoenix::gles2::texture::Cube::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_object);
}

#endif

#include "gles2-txt-cube.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"

#define FACES_COUNT 6

gearoenix::gles2::texture::Cube::Cube(system::File* file, Engine* eng, std::shared_ptr<core::EndCaller> end)
{
    std::vector<unsigned char> img_data[FACES_COUNT];
    unsigned int imgw[FACES_COUNT], imgh[FACES_COUNT], imgc[FACES_COUNT];
    std::vector<core::Offset> img_offs(5);
    for (int i = 0; i < 5; ++i)
        file->read(img_offs[i]);
    render::texture::PNG::decode(file, eng, img_data[0], imgw[0], imgh[0], imgc[0]);
    for (int i = 1; i < FACES_COUNT; ++i) {
        file->seek(img_offs[i - 1]);
        render::texture::PNG::decode(file, eng, img_data[i], imgw[i], imgh[i], imgc[i]);
    }
    std::function<void()> loadf = [this, imgw, imgh, imgc, img_data, end] {

        glGenTextures(1, &texture_object);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_object);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_MIRRORED_REPEAT
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        const GLenum faces[] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        };
        for (int i = 0; i < FACES_COUNT; ++i) {
            switch (imgc[i]) {
            case 4:
                glTexImage2D(faces[i], 0, GL_RGBA, imgw[i], imgh[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)img_data[i].data());
                break;
            case 3:
                glTexImage2D(faces[i], 0, GL_RGB, imgw[i], imgh[i], 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)img_data[i].data());
                break;
            default:
                UNEXPECTED;
            }
        }
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        (void)end;
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

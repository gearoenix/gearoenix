#include "gles2-txt-cube.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

#define GX_FACES_COUNT 6

gearoenix::gles2::texture::Cube::Cube(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& file,
    const std::shared_ptr<engine::Engine>& engine,
    core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : render::texture::Cube(my_id, engine)
{
    std::vector<std::vector<unsigned char>> img_data(GX_FACES_COUNT);
    unsigned int imgw, imgh;
    render::texture::PNG::decode(file, img_data[0], imgw, imgh);
    for (int i = 1; i < GX_FACES_COUNT; ++i) {
        unsigned int iimgw, iimgh;
        render::texture::PNG::decode(file, img_data[i], iimgw, iimgh);
        if (iimgw != imgw || iimgh != imgh)
            GXUNEXPECTED;
    }
    static const gl::enumerated faces[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    };
    std::function<void()> loadf = [&, imgw, imgh, img_data, end]() -> void {
        gl::Loader::gen_textures(1, &texture_object);
        gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, texture_object);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl::Loader::tex_parameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_MllIRRORED_REPEAT
        gl::Loader::tex_parameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        for (int i = 0; i < GX_FACES_COUNT; ++i) {
            gl::Loader::tex_image_2d(faces[i], 0, GL_RGBA, imgw, imgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)img_data[i].data());
        }
        gl::Loader::generate_mipmap(GL_TEXTURE_CUBE_MAP);
        gl::Loader::get_error();
        //GX_CHECK_FOR_GRAPHIC_API_ERROR
        (void)end;
    };
//    engine->add_load_function(loadf);
}

gearoenix::gles2::texture::Cube::~Cube()
{
    if (texture_object == 0)
        return;
    const gl::uint c_texture_object = texture_object;
//    render_engine->add_load_function([c_texture_object] {
//        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//        glDeleteTextures(1, &c_texture_object);
//    });
    texture_object = 0;
}

void gearoenix::gles2::texture::Cube::bind(gl::enumerated texture_unit) const
{
    gl::Loader::active_texture(texture_unit);
    gl::Loader::bind_texture(GL_TEXTURE_CUBE_MAP, texture_object);
}

#endif

#include "rnd-txt-texture.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../rnd-engine.hpp"
#include "rnd-txt-texture-2d.hpp"
#include "rnd-txt-texture-cube.hpp"

gearoenix::render::texture::Texture::~Texture() {}

gearoenix::render::texture::Texture* gearoenix::render::texture::Texture::read(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    //    LOGE("textell: " << f->tell());
    core::Id t;
    f->read(t);
    switch (t) {
    case 1:
    case 5:
    case 6:
        return e->create_texture_2d(f, c);
    case 3:
    case 4:
        return e->create_texture_cube(f, c);
    default:
        UNEXPECTED;
    }
    return nullptr;
}

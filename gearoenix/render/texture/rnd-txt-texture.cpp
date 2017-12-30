#include "rnd-txt-texture.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../rnd-engine.hpp"
#include "rnd-txt-texture-2d.hpp"
#include "rnd-txt-texture-cube.hpp"

gearoenix::render::texture::Texture::~Texture() {}

gearoenix::render::texture::Texture* gearoenix::render::texture::Texture::read(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    //    LOGE("textell: " << f->tell());
    core::Id t;
    f->read(t);
    switch (t) {
    case 10:
        return e->create_texture_2d(f, c);
    case 20: {
        return e->create_texture_cube(f, c);
    }
    }
    UNEXPECTED;
}

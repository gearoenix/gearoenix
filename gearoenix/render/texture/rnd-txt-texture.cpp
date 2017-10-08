#include "rnd-txt-texture.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../rnd-engine.hpp"
#include "rnd-txt-texture-2d.hpp"

gearoenix::render::texture::Texture::~Texture() {}

gearoenix::render::texture::Texture* gearoenix::render::texture::Texture::read(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    LOGE("textell: " << f->tell());
    core::Id t;
    f->read(t);
    switch (t) {
    case 10:
        return new Texture2D(f, e, c);
    case 20: {
        LOGF("Cube texture is not implemented yet.");
    }
    }
    LOGF("Unexected");
}

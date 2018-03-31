#include "rnd-txt-texture.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../rnd-engine.hpp"
#include "rnd-txt-texture-2d.hpp"
#include "rnd-txt-texture-cube.hpp"

gearoenix::render::texture::Texture::Texture(core::Id my_id, Engine* e)
    : core::asset::Asset(my_id, core::asset::Asset::AssetType::TEXTURE)
    , render_engine(e)
{
}

gearoenix::render::texture::Texture::~Texture() {}

gearoenix::render::texture::Texture* gearoenix::render::texture::Texture::read(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    //    LOGE("textell: " << f->tell());
    core::Id t;
    f->read(t);
    switch (t) {
    case 1:
    case 5:
    case 6:
        return e->create_texture_2d(my_id, f, c);
    case 3:
    case 4:
        return e->create_texture_cube(my_id, f, c);
    default:
        UNEXPECTED;
    }
    return nullptr;
}

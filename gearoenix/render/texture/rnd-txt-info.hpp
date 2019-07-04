#ifndef GEAROENIX_RENDER_TEXTURE_INFO_HPP
#define GEAROENIX_RENDER_TEXTURE_INFO_HPP
#include "rnd-txt-format.hpp"
#include "rnd-txt-sample.hpp"
#include "rnd-txt-type.hpp"
namespace gearoenix::render::texture {
struct Info {
    TextureFormat::Id f = TextureFormat::RGBA_UINT8;
    SampleInfo s;
    Type::Id t = Type::TEXTURE_2D;
};
}
#endif

#ifndef GEAROENIX_RENDER_TEXTURE_INFO_HPP
#define GEAROENIX_RENDER_TEXTURE_INFO_HPP
#include "rnd-txt-format.hpp"
#include "rnd-txt-sample.hpp"
#include "rnd-txt-type.hpp"
namespace gearoenix::render::texture {
struct Info {
    TextureFormat f = TextureFormat::RgbaUint8;
    SampleInfo s;
    Type t = Type::Texture2D;
};
}
#endif

#ifndef GEAROENIX_RENDER_TEXTURE_INFO_HPP
#define GEAROENIX_RENDER_TEXTURE_INFO_HPP
#include "gx-rnd-txt-format.hpp"
#include "gx-rnd-txt-sample.hpp"
#include "gx-rnd-txt-type.hpp"

namespace gearoenix::render::texture {
struct TextureInfo {
    TextureFormat format = TextureFormat::RgbaUint8;
    SampleInfo sample_info;
    Type texture_type = Type::Texture2D;
    bool has_mipmap = true;
};
}
#endif

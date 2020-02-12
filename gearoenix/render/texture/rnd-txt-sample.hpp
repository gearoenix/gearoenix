#ifndef GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#define GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#include "rnd-txt-filter.hpp"
#include "rnd-txt-wrap.hpp"
namespace gearoenix::render::texture {
struct SampleInfo {
    Filter min_filter = Filter::LINEAR_MIPMAP_LINEAR;
    Filter mag_filter = Filter::LINEAR;
    Wrap wrap_s = Wrap::Repeat;
    Wrap wrap_t = Wrap::Repeat;
    Wrap wrap_r = Wrap::Repeat;
};
}
#endif

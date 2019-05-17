#ifndef GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#define GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#include "rnd-txt-filter.hpp"
#include "rnd-txt-wrap.hpp"
namespace gearoenix {
namespace render {
    namespace texture {
        struct SampleInfo {
            Filter::Id min_filter = Filter::LINEAR;
            Filter::Id mag_filter = Filter::LINEAR;
            Wrap::Id wrap_s = Wrap::REPEAT;
            Wrap::Id wrap_t = Wrap::REPEAT;
            Wrap::Id wrap_r = Wrap::REPEAT;
        };
    }
}
}
#endif

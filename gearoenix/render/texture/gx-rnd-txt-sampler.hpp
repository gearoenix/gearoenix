#ifndef GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#define GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#include "gx-rnd-txt-filter.hpp"
#include "gx-rnd-txt-wrap.hpp"

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::texture {
struct SamplerInfo {
    Filter min_filter = Filter::LinearMipmapLinear;
    Filter mag_filter = Filter::Linear;
    Wrap wrap_s = Wrap::Repeat;
    Wrap wrap_t = Wrap::Repeat;
    Wrap wrap_r = Wrap::Repeat;
    std::uint8_t anisotropic_level = 1;

    void write(system::stream::Stream* s) const noexcept;
    void read(system::stream::Stream* s) noexcept;
    [[nodiscard]] bool needs_mipmap() const noexcept;
};
}
#endif

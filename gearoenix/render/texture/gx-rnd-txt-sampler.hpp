#ifndef GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#define GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#include "gx-rnd-txt-filter.hpp"
#include "gx-rnd-txt-wrap.hpp"
#include <cstdlib>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct SamplerInfo final {
    Filter min_filter = Filter::LinearMipmapLinear;
    Filter mag_filter = Filter::Linear;
    Wrap wrap_s = Wrap::Repeat;
    Wrap wrap_t = Wrap::Repeat;
    Wrap wrap_r = Wrap::Repeat;
    std::uint8_t anisotropic_level = 0;

    void write(platform::stream::Stream& s) const;
    void read(platform::stream::Stream& s);
    [[nodiscard]] bool needs_mipmap() const;
    [[nodiscard]] bool operator<(const SamplerInfo& o) const;
    [[nodiscard]] bool operator>(const SamplerInfo& o) const;
    [[nodiscard]] bool operator==(const SamplerInfo& o) const;
};

struct SamplerInfoHasher final {
    [[nodiscard]] std::size_t operator()(const SamplerInfo& sampler) const;
};
}
#endif

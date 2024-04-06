#ifndef GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#define GEAROENIX_RENDER_TEXTURE_SAMPLE_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-rnd-txt-filter.hpp"
#include "gx-rnd-txt-wrap.hpp"
#include <cstdlib>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct SamplerInfo final {
    GX_GET_VAL_PRV(std::size_t, hash, 0);
    GX_GET_VAL_PRV(Filter, min_filter, Filter::LinearMipmapLinear);
    GX_GET_VAL_PRV(Filter, mag_filter, Filter::Linear);
    GX_GET_VAL_PRV(Wrap, wrap_s, Wrap::Repeat);
    GX_GET_VAL_PRV(Wrap, wrap_t, Wrap::Repeat);
    GX_GET_VAL_PRV(Wrap, wrap_r, Wrap::Repeat);
    GX_GET_VAL_PRV(std::uint8_t, anisotropic_level, 0);

public:
    explicit SamplerInfo(
        Filter min_filter = Filter::LinearMipmapLinear,
        Filter mag_filter = Filter::Linear,
        Wrap wrap_s = Wrap::Repeat,
        Wrap wrap_t = Wrap::Repeat,
        Wrap wrap_r = Wrap::Repeat,
        std::uint8_t anisotropic_level = 0);

    void write(platform::stream::Stream& s) const;
    void read(platform::stream::Stream& s);
    [[nodiscard]] bool needs_mipmap() const;
    [[nodiscard]] bool operator<(const SamplerInfo& o) const;
    [[nodiscard]] bool operator>(const SamplerInfo& o) const;
    [[nodiscard]] bool operator==(const SamplerInfo& o) const;
    SamplerInfo& set_min_filter(Filter v);
    SamplerInfo& set_mag_filter(Filter v);
    SamplerInfo& set_wrap_s(Wrap v);
    SamplerInfo& set_wrap_t(Wrap v);
    SamplerInfo& set_wrap_r(Wrap v);
    SamplerInfo& set_anisotropic_level(std::uint8_t v);
    void update_hash();
};

struct SamplerInfoHasher final {
    [[nodiscard]] inline std::size_t operator()(const SamplerInfo& sampler) const { return sampler.get_hash(); }
};
}
#endif

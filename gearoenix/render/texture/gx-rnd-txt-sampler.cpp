#include "gx-rnd-txt-sampler.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

void gearoenix::render::texture::SamplerInfo::write(platform::stream::Stream* const s) const noexcept
{
    (void)s->write(min_filter);
    (void)s->write(mag_filter);
    (void)s->write(wrap_s);
    (void)s->write(wrap_t);
    (void)s->write(wrap_r);
}

void gearoenix::render::texture::SamplerInfo::read(platform::stream::Stream* const s) noexcept
{
    s->read(min_filter);
    s->read(mag_filter);
    s->read(wrap_s);
    s->read(wrap_t);
    s->read(wrap_r);
}

bool gearoenix::render::texture::SamplerInfo::needs_mipmap() const noexcept
{
    return min_filter == Filter::CubicMipmapCubic || min_filter == Filter::CubicMipmapLinear || min_filter == Filter::CubicMipmapNearest || min_filter == Filter::LinearMipmapCubic || min_filter == Filter::LinearMipmapLinear || min_filter == Filter::LinearMipmapNearest || min_filter == Filter::NearestMipmapCubic || min_filter == Filter::NearestMipmapLinear || min_filter == Filter::NearestMipmapNearest;
}

bool gearoenix::render::texture::SamplerInfo::operator<(const SamplerInfo& o) const noexcept
{
#define GX_HELPER(x) \
    if (x < o.x)     \
        return true; \
    if (x > o.x)     \
        return false;
    GX_HELPER(min_filter)
    GX_HELPER(mag_filter)
    GX_HELPER(wrap_s)
    GX_HELPER(wrap_r)
    GX_HELPER(wrap_t)
    GX_HELPER(anisotropic_level)
    return false;
#undef GX_HELPER
}

bool gearoenix::render::texture::SamplerInfo::operator==(const SamplerInfo& o) const noexcept
{
#define GX_HELPER(x) (x == o.x)
    return GX_HELPER(min_filter) || GX_HELPER(mag_filter) || GX_HELPER(wrap_s) || GX_HELPER(wrap_r) || GX_HELPER(wrap_t) || GX_HELPER(anisotropic_level);
#undef GX_HELPER
}

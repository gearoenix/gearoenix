#include "gx-rnd-txt-sampler.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include <boost/functional/hash.hpp>

gearoenix::render::texture::SamplerInfo::SamplerInfo(
    const Filter min_filter,
    const Filter mag_filter,
    const Wrap wrap_s,
    const Wrap wrap_t,
    const Wrap wrap_r,
    const std::uint8_t anisotropic_level)
    : min_filter(min_filter)
    , mag_filter(mag_filter)
    , wrap_s(wrap_s)
    , wrap_t(wrap_t)
    , wrap_r(wrap_r)
    , anisotropic_level(anisotropic_level)
{
    update_hash();
}

void gearoenix::render::texture::SamplerInfo::write(platform::stream::Stream& s) const
{
    s.write_fail_debug(min_filter);
    s.write_fail_debug(mag_filter);
    s.write_fail_debug(wrap_s);
    s.write_fail_debug(wrap_t);
    s.write_fail_debug(wrap_r);
    s.write_fail_debug(anisotropic_level);
}

void gearoenix::render::texture::SamplerInfo::read(platform::stream::Stream& s)
{
    s.read(min_filter);
    s.read(mag_filter);
    s.read(wrap_s);
    s.read(wrap_t);
    s.read(wrap_r);
    s.read(anisotropic_level);
    update_hash();
}

bool gearoenix::render::texture::SamplerInfo::needs_mipmap() const
{
    return min_filter == Filter::CubicMipmapCubic || min_filter == Filter::CubicMipmapLinear || min_filter == Filter::CubicMipmapNearest || min_filter == Filter::LinearMipmapCubic || min_filter == Filter::LinearMipmapLinear || min_filter == Filter::LinearMipmapNearest || min_filter == Filter::NearestMipmapCubic || min_filter == Filter::NearestMipmapLinear || min_filter == Filter::NearestMipmapNearest;
}

bool gearoenix::render::texture::SamplerInfo::operator<(const SamplerInfo& o) const
{
    if (hash > o.hash) {
        return false;
    }
    if (hash < o.hash) {
        return true;
    }
    if (min_filter > o.min_filter) {
        return false;
    }
    if (min_filter < o.min_filter) {
        return true;
    }
    if (mag_filter > o.mag_filter) {
        return false;
    }
    if (mag_filter < o.mag_filter) {
        return true;
    }
    if (wrap_s > o.wrap_s) {
        return false;
    }
    if (wrap_s < o.wrap_s) {
        return true;
    }
    if (wrap_r > o.wrap_r) {
        return false;
    }
    if (wrap_r < o.wrap_r) {
        return true;
    }
    if (wrap_t > o.wrap_t) {
        return false;
    }
    if (wrap_t < o.wrap_t) {
        return true;
    }
    if (anisotropic_level > o.anisotropic_level) {
        return false;
    }
    if (anisotropic_level < o.anisotropic_level) {
        return true;
    }
    return false;
}

bool gearoenix::render::texture::SamplerInfo::operator>(const SamplerInfo& o) const
{
    if (hash > o.hash) {
        return true;
    }
    if (hash < o.hash) {
        return false;
    }
    if (min_filter > o.min_filter) {
        return true;
    }
    if (min_filter < o.min_filter) {
        return false;
    }
    if (mag_filter > o.mag_filter) {
        return true;
    }
    if (mag_filter < o.mag_filter) {
        return false;
    }
    if (wrap_s > o.wrap_s) {
        return true;
    }
    if (wrap_s < o.wrap_s) {
        return false;
    }
    if (wrap_r > o.wrap_r) {
        return true;
    }
    if (wrap_r < o.wrap_r) {
        return false;
    }
    if (wrap_t > o.wrap_t) {
        return true;
    }
    if (wrap_t < o.wrap_t) {
        return false;
    }
    if (anisotropic_level > o.anisotropic_level) {
        return true;
    }
    if (anisotropic_level < o.anisotropic_level) {
        return false;
    }
    return false;
}

bool gearoenix::render::texture::SamplerInfo::operator==(const SamplerInfo& o) const
{
    return hash == o.hash && min_filter == o.min_filter && mag_filter == o.mag_filter && wrap_s == o.wrap_s && wrap_r == o.wrap_r && wrap_t == o.wrap_t && anisotropic_level == o.anisotropic_level;
}

gearoenix::render::texture::SamplerInfo &gearoenix::render::texture::SamplerInfo::set_min_filter(const Filter v) {
    if(v == min_filter) {
        return *this;
    }
    min_filter = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::SamplerInfo &gearoenix::render::texture::SamplerInfo::set_mag_filter(const Filter v) {
    if(v == mag_filter) {
        return *this;
    }
    mag_filter = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::SamplerInfo &gearoenix::render::texture::SamplerInfo::set_wrap_s(const Wrap v) {
    if(v == wrap_s) {
        return *this;
    }
    wrap_s = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::SamplerInfo &gearoenix::render::texture::SamplerInfo::set_wrap_t(const Wrap v) {
    if(v == wrap_t) {
        return *this;
    }
    wrap_t = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::SamplerInfo &gearoenix::render::texture::SamplerInfo::set_wrap_r(const Wrap v) {
    if(v == wrap_r) {
        return *this;
    }
    wrap_r = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::SamplerInfo &gearoenix::render::texture::SamplerInfo::set_anisotropic_level(const std::uint8_t v) {
    if(v == anisotropic_level) {
        return *this;
    }
    anisotropic_level = v;
    update_hash();
    return *this;
}

void gearoenix::render::texture::SamplerInfo::update_hash() {
    hash = boost::hash_value(typeid(SamplerInfo));
    boost::hash_combine(hash, boost::hash_value(min_filter));
    boost::hash_combine(hash, boost::hash_value(mag_filter));
    boost::hash_combine(hash, boost::hash_value(wrap_s));
    boost::hash_combine(hash, boost::hash_value(wrap_t));
    boost::hash_combine(hash, boost::hash_value(wrap_r));
    boost::hash_combine(hash, boost::hash_value(anisotropic_level));
}

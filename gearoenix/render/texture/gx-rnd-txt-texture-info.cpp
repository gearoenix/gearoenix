#include "gx-rnd-txt-texture-info.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include <boost/functional/hash.hpp>

bool gearoenix::render::texture::TextureInfo::operator==(const TextureInfo& o) const noexcept = default;

void gearoenix::render::texture::TextureInfo::read(platform::stream::Stream& s) noexcept
{
    s.read(format);
    sampler_info.read(s);
    s.read(width);
    s.read(height);
    s.read(depth);
    s.read(type);
    s.read(has_mipmap);
}

void gearoenix::render::texture::TextureInfo::write(platform::stream::Stream& s) const noexcept
{
    s.write_fail_debug(format);
    sampler_info.write(s);
    s.write_fail_debug(width);
    s.write_fail_debug(height);
    s.write_fail_debug(depth);
    s.write_fail_debug(type);
    s.write_fail_debug(has_mipmap);
}

std::size_t gearoenix::render::texture::TextureInfoHasher::operator()(const TextureInfo& i) const noexcept
{
    std::size_t seed = boost::hash_value(typeid(SamplerInfo));
    boost::hash_combine(seed, boost::hash_value(i.format));
    boost::hash_combine(seed, SamplerInfoHasher()(i.sampler_info));
    boost::hash_combine(seed, boost::hash_value(i.width));
    boost::hash_combine(seed, boost::hash_value(i.height));
    boost::hash_combine(seed, boost::hash_value(i.depth));
    boost::hash_combine(seed, boost::hash_value(i.type));
    boost::hash_combine(seed, boost::hash_value(i.has_mipmap));
    return seed;
}

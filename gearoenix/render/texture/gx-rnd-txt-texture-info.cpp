#include "gx-rnd-txt-texture-info.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

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

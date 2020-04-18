#include "rnd-txt-sample.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

void gearoenix::render::texture::SampleInfo::write(system::stream::Stream* const s) const noexcept
{
    (void)s->write(min_filter);
    (void)s->write(mag_filter);
    (void)s->write(wrap_s);
    (void)s->write(wrap_t);
    (void)s->write(wrap_r);
}

void gearoenix::render::texture::SampleInfo::read(system::stream::Stream* const s) noexcept
{
    s->read(min_filter);
    s->read(mag_filter);
    s->read(wrap_s);
    s->read(wrap_t);
    s->read(wrap_r);
}
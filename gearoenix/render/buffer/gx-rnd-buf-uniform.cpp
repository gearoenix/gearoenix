#include "gx-rnd-buf-uniform.hpp"
#include "../../core/allocator/gx-cr-alc-range.hpp"

gearoenix::render::buffer::Uniform::Uniform(std::shared_ptr<core::allocator::Range>&& range)
    : range(std::move(range))
    , size(range->get_size())
    , offset_from_frame_pointer(range->get_offset())
{
}

gearoenix::render::buffer::Uniform::Uniform(const Uniform& o)
    : range(o.range)
    , size(range->get_size())
    , offset_from_frame_pointer(range->get_offset())
{
}

gearoenix::render::buffer::Uniform::~Uniform() = default;
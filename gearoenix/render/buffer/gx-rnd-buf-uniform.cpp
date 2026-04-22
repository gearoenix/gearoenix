#include "gx-rnd-buf-uniform.hpp"
#include "../../core/allocator/gx-cr-alc-range.hpp"

std::size_t gearoenix::render::buffer::Uniform::frame_pointer;

gearoenix::render::buffer::Uniform::Uniform(cns_args_t&& args)
    : range(std::move(args.first))
    , size(range->get_size())
    , offset_from_frame_pointer(range->get_offset())
    , shader_data_index(args.second)
{
}

gearoenix::render::buffer::Uniform::~Uniform() = default;
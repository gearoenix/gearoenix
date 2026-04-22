#include "gx-gl-buf-manager.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"

std::size_t gearoenix::gl::buffer::Manager::get_uniform_frame_pointer()
{
    return reinterpret_cast<std::size_t>(uniform_buffer_data.data());
}

gearoenix::gl::buffer::Manager::Manager()
    : Singleton<Manager>(this)
{
    uniform_internal_range_alignment = 1;
    uniform_range_alignment = 1;

    initialise_ranges();

    GX_ASSERT_D(uniform_root_range);
    GX_ASSERT_D(uniform_root_range->get_size() > 0);

    uniform_buffer_data.resize(uniform_root_range->get_size());
}

gearoenix::gl::buffer::Manager::~Manager() = default;

#endif
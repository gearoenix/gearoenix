#include "gx-gl-buf-manager.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../render/gx-rnd-runtime-configuration.hpp"
#include "../gx-gl-check.hpp"
#include "../gx-gl-loader.hpp"

gearoenix::gl::buffer::Manager::Manager()
    : Singleton<Manager>(this)
    , per_frame_size(static_cast<std::int64_t>(render::RuntimeConfiguration::get().get_maximum_allowed_dynamic_buffer_size()))
    , uniform_offset_alignment([]-> std::int64_t {
        sint a = 0;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &a);
        if (a < 1 || a > 1024) {
            return 256;
        }
        return static_cast<std::int64_t>(a);
    }())
    , root_of_ranges(core::allocator::Range::construct(per_frame_size))
#if GX_PLATFORM_WEBASSEMBLY
    , buffer_data_holder(per_frame_size)
#endif
{
    GX_ASSERT_D(per_frame_size > 0);
    glGenBuffers(static_cast<sizei>(root_uniform_buffers.size()), root_uniform_buffers.data());
    for (const auto gpu_buffer : root_uniform_buffers) {
        GX_ASSERT_D(gpu_buffer != 0);
        glBindBuffer(GL_UNIFORM_BUFFER, gpu_buffer);
        glBufferData(GL_UNIFORM_BUFFER, static_cast<sizeiptr>(per_frame_size), nullptr, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    GX_GL_CHECK_D;
}

gearoenix::gl::buffer::Manager::~Manager()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glDeleteBuffers(static_cast<sizei>(root_uniform_buffers.size()), root_uniform_buffers.data());
}

gearoenix::render::buffer::Uniform gearoenix::gl::buffer::Manager::create_uniform(const std::int64_t size)
{
    auto range = root_of_ranges->allocate(size, uniform_offset_alignment);
    GX_ASSERT_D(range);
    {
        const auto end = range->get_offset() + size;
        const std::lock_guard _(lock);
        max_end = std::max(max_end, static_cast<std::size_t>(end));
    }
    return render::buffer::Uniform(std::move(range));
}

void gearoenix::gl::buffer::Manager::start_frame()
{
    invalid_frame = true;
    if (max_end == 0) {
        return;
    }
    current_frame = render::engine::Engine::get_frame_number_from_start() % root_uniform_buffers.size();
#if GX_PLATFORM_WEBASSEMBLY
    render::buffer::Uniform::set_frame_pointer(buffer_data_holder.data());
#else
    glBindBuffer(GL_UNIFORM_BUFFER, root_uniform_buffers[current_frame]);
    void* const ptr = glMapBufferRange(
        GL_UNIFORM_BUFFER, 0, static_cast<sizeiptr>(max_end),
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    GX_ASSERT_D(ptr != nullptr);
    render::buffer::Uniform::set_frame_pointer(reinterpret_cast<std::size_t>(ptr));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    GX_GL_CHECK_D;
#endif
    invalid_frame = false;
}

void gearoenix::gl::buffer::Manager::end_frame()
{
    if (invalid_frame) {
        return;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, root_uniform_buffers[current_frame]);
#ifdef GX_PLATFORM_WEBASSEMBLY
    glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<sizeiptr>(max_end), buffer_data_holder.data());
#else
    glUnmapBuffer(GL_UNIFORM_BUFFER);
#endif
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    GX_GL_CHECK_D;
    invalid_frame = true;
}

#endif
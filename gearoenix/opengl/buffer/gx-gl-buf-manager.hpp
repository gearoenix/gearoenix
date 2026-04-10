#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../platform/gx-plt-build-configuration.hpp"
#include "../../render/buffer/gx-rnd-buf-manager.hpp"
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "../gx-gl-constants.hpp"
#include "../gx-gl-types.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>
#include <mutex>

namespace gearoenix::core::allocator {
struct Range;
}

namespace gearoenix::gl::buffer {
struct Manager final : render::buffer::Manager, core::Singleton<Manager> {
private:
    const std::int64_t per_frame_size;
    const std::int64_t uniform_offset_alignment;
    std::mutex lock;
    std::size_t max_end = 0; // in C++26, we can use atomic_max. For now, we are going to use mutex.
    const std::shared_ptr<core::allocator::Range> root_of_ranges;

    // +1 slot: with an N-deep swap chain, mainstream drivers queue at most N+1
    // frames of commands ahead of GPU execution, so an (N+1)-slot UBO rotation
    // guarantees the slot we're about to map is no longer referenced by any
    // in-flight GPU work. This is what makes GL_MAP_UNSYNCHRONIZED_BIT safe below
    // without an explicit glFenceSync.
    std::array<uint, GEAROENIX_GL_FRAMES_COUNT + 1> root_uniform_buffers { };
    bool invalid_frame = false;
    std::size_t current_frame = 0;

#if GX_PLATFORM_WEBASSEMBLY
    std::vector<std::uint8_t> buffer_data_holder;
#endif

public:
    Manager();
    ~Manager() override;

    [[nodiscard]] render::buffer::Uniform create_uniform(std::int64_t size);

    /// Called at the start of each frame, maps (or prepares) the current
    /// frame's CPU-visible memory so record code can write into it directly.
    void start_frame();

    /// Called at the end of each frame, unmaps or uploads the current frame's
    /// memory so it becomes visible to the GPU for the subsequent draws.
    void end_frame();

    [[nodiscard]] uint get_gpu_buffer(const std::uint64_t frame) const { return root_uniform_buffers[frame]; }
};
}
#endif
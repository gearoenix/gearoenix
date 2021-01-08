#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-buf-buffer.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform final {
    const std::size_t size;
    const std::size_t frame_number;

private:
    vulkan::buffer::Buffer base;

public:
    Uniform(vulkan::buffer::Buffer&& base, std::size_t size, std::size_t frame_number) noexcept;
    ~Uniform() noexcept = default;
    void update(const void*) noexcept;
};
}
#endif
#endif

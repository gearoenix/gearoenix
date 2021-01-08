#ifndef GEAROENIX_VULKAN_BUFFER_STATIC_HPP
#define GEAROENIX_VULKAN_BUFFER_STATIC_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-buf-buffer.hpp"

namespace gearoenix::vulkan::buffer {
struct Static final {
    const std::size_t size;

private:
    vulkan::buffer::Buffer base;

public:
    Static(vulkan::buffer::Buffer&& base, std::size_t size) noexcept;
    ~Static() noexcept = default;
};
}
#endif
#endif

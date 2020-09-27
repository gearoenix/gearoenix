#ifndef GEAROENIX_VULKAN_BUFFER_STATIC_HPP
#define GEAROENIX_VULKAN_BUFFER_STATIC_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../render/buffer/gx-rnd-buf-static.hpp"
#include "gx-vk-buf-buffer.hpp"

namespace gearoenix::vulkan::buffer {
class Static final : public render::buffer::Static, public Buffer {
public:
    Static(
        std::size_t size,
        std::uint32_t offset,
        VkBuffer vulkan_data,
        std::shared_ptr<memory::Memory> allocated_memory,
        engine::Engine* eng) noexcept;
    ~Static() noexcept final;
};
}
#endif
#endif

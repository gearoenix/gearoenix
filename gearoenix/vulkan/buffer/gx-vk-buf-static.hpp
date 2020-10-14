#ifndef GEAROENIX_VULKAN_BUFFER_STATIC_HPP
#define GEAROENIX_VULKAN_BUFFER_STATIC_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../render/buffer/gx-rnd-buf-static.hpp"

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::buffer {
class Buffer;
class Static final : public render::buffer::Static {
    GX_GET_REFC_PRV(std::shared_ptr<vulkan::buffer::Buffer>, allocated_buffer)
public:
    Static(
        std::shared_ptr<vulkan::buffer::Buffer> allocated_buffer,
        std::size_t size,
        engine::Engine* eng) noexcept;
    ~Static() noexcept final;
};
}
#endif
#endif

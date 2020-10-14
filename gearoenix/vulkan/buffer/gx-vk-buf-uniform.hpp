#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::buffer {
class Buffer;
class Uniform final : public render::buffer::Uniform {
    GX_GET_REFC_PRV(std::shared_ptr<vulkan::buffer::Buffer>, allocated_buffer)
private:
    [[nodiscard]] const void* get_data() const noexcept final;
    [[nodiscard]] void* get_data() noexcept final;

public:
    Uniform(
        std::shared_ptr<vulkan::buffer::Buffer> allocated_buffer,
        std::size_t size,
        std::size_t frame_number,
        engine::Engine* eng) noexcept;
    ~Uniform() noexcept final;
    void update(const void*) noexcept final;
};
}
#endif
#endif

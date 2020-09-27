#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "gx-vk-buf-buffer.hpp"

namespace gearoenix::vulkan::buffer {
class Uniform final : public render::buffer::Uniform, public Buffer {
private:
    [[nodiscard]] const void* get_data() const noexcept final;
    [[nodiscard]] void* get_data() noexcept final;

public:
    Uniform(
        std::size_t size,
        std::uint32_t offset,
        VkBuffer vulkan_data,
        std::shared_ptr<memory::Memory> allocated_memory,
        void* data,
        engine::Engine* eng) noexcept;
    ~Uniform() noexcept final;
    void update(const void*) noexcept final;
};
}
#endif
#endif

#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::memory {
class Memory;
}

namespace gearoenix::vulkan::buffer {
class Uniform final : public render::buffer::Uniform {
    GX_GET_VAL_PRV(VkBuffer, buffer, nullptr)
    GX_GET_REFC_PRV(std::shared_ptr<memory::Memory>, memory)
private:
    void* const data;

    [[nodiscard]] const void* get_data() const noexcept final;
    [[nodiscard]] void* get_data() noexcept final;
    void update(const void*) noexcept final;

public:
    Uniform(std::size_t size, VkBuffer buffer, std::shared_ptr<memory::Memory> memory, void* data, engine::Engine* eng) noexcept;
    ~Uniform() noexcept final;
};
}
#endif
#endif

#ifndef GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#define GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../../render/buffer/gx-rnd-buf-manager.hpp"

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::memory {
class Manager;
}

namespace gearoenix::vulkan::buffer {
class Manager final : public render::buffer::Manager {
    GX_GET_REFC_PRV(std::shared_ptr<memory::Manager>, memory_manager)
public:
    Manager(std::shared_ptr<memory::Manager> memory_manager, engine::Engine* eng) noexcept;
    ~Manager() noexcept final;
    [[nodiscard]] render::buffer::Uniform* create_uniform(std::size_t size) noexcept final;
    [[nodiscard]] render::buffer::Static* create_static(const std::vector<math::BasicVertex>& vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
    [[nodiscard]] render::buffer::Static* create_static(const std::vector<std::uint32_t>& indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
};
}
#endif
#endif

#ifndef GEAROENIX_VULKAN_MESH_ACCEL_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::mesh {
struct Manager;
struct Accel final : public render::mesh::Mesh {
    friend Manager;
    GX_GET_CREF_PRV(buffer::Buffer, vertex)
    GX_GET_CREF_PRV(buffer::Buffer, index)

private:
    VkAccelerationStructureKHR vulkan_data = nullptr;
    std::optional<buffer::Buffer> accel = std::nullopt;

    std::weak_ptr<Accel> self;

    Accel(engine::Engine&, std::vector<math::BasicVertex> vertices, std::vector<std::uint32_t> indices) noexcept;

public:
    Accel(Accel&&) = delete;
    Accel(const Accel&) = delete;
    Accel& operator=(Accel&&) = delete;
    Accel& operator=(const Accel&) = delete;
    [[nodiscard]] static std::shared_ptr<Accel> construct(
        engine::Engine&, std::vector<math::BasicVertex> vertices, std::vector<std::uint32_t> indices) noexcept;
    ~Accel() noexcept final;
    void set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept final;
    [[nodiscard]] std::pair<VkDeviceAddress, VkDeviceAddress> get_buffers_address() const noexcept;
};
}

#endif
#endif
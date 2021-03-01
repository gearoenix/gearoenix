#ifndef GEAROENIX_VULKAN_MESH_ACCEL_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../math/gx-math-vertex.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::mesh {
struct Manager;
struct Accel final : public render::mesh::Mesh {
    friend Manager;
    GX_GET_REFC_PRV(std::shared_ptr<buffer::Buffer>, vertex)
    GX_GET_REFC_PRV(std::shared_ptr<buffer::Buffer>, index)

private:
    VkAccelerationStructureKHR vulkan_data = nullptr;
    std::shared_ptr<buffer::Buffer> accel = nullptr;

    std::weak_ptr<Accel> self;

    Accel(std::shared_ptr<buffer::Buffer> vertex, std::shared_ptr<buffer::Buffer> index) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Accel> construct(
        engine::Engine&,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices) noexcept;
    ~Accel() noexcept final;
    void set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept final;
    [[nodiscard]] std::pair<VkDeviceAddress, VkDeviceAddress> get_buffers_address() const noexcept;
};
}

#endif
#endif
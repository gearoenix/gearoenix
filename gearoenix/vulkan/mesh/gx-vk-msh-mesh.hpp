#ifndef GEAROENIX_VULKAN_MESH_ACCEL_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../render/gx-rnd-vertex.hpp"
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
struct Mesh final : public render::mesh::Mesh {
    friend struct Manager;

    GX_GET_REFC_PRV(std::shared_ptr<buffer::Buffer>, vertex);
    GX_GET_REFC_PRV(std::shared_ptr<buffer::Buffer>, index);
    GX_GET_CREF_PRV(std::shared_ptr<buffer::Buffer>, accel_buff);
    GX_GET_VAL_PRV(VkAccelerationStructureKHR, vulkan_data, nullptr);
    GX_GET_VAL_PRV(VkDeviceAddress, acceleration_address, 0);

private:
    void initialize_blas() noexcept;

public:
    Mesh(
        engine::Engine& e,
        const std::string& name,
        const render::Vertices& vertices,
        const std::vector<std::uint32_t>& indices,
        math::Aabb3<double>&& occlusion_box,
        const core::sync::EndCallerIgnored& end) noexcept;
    ~Mesh() noexcept final;
    [[nodiscard]] std::pair<VkDeviceAddress, VkDeviceAddress> get_buffers_address() const noexcept;
};
}

#endif
#endif
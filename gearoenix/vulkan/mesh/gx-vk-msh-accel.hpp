#ifndef GEAROENIX_VULKAN_MESH_ACCEL_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
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
struct AccelManager;
struct Accel final : public render::mesh::Mesh {
    friend AccelManager;
    GX_GET_REFC_PRV(std::shared_ptr<buffer::Buffer>, vertex)
    GX_GET_REFC_PRV(std::shared_ptr<buffer::Buffer>, index)
    GX_GET_VAL_PRV(VkAccelerationStructureKHR, vulkan_data, nullptr)
    GX_GET_VAL_PRV(VkDeviceAddress, acceleration_address, 0)

private:
    std::shared_ptr<buffer::Buffer> accel_buff = nullptr;

    std::weak_ptr<Accel> self;

    Accel(std::shared_ptr<buffer::Buffer> vertex, std::shared_ptr<buffer::Buffer> index) noexcept;
    void initialize_blas() noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Accel> construct(
        engine::Engine&,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<Accel>&) noexcept;
    ~Accel() noexcept final;
    void set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept final;
    [[nodiscard]] std::pair<VkDeviceAddress, VkDeviceAddress> get_buffers_address() const noexcept;
};
}

#endif
#endif
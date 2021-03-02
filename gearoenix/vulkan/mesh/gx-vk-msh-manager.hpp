#ifndef GEAROENIX_VULKAN_MESH_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::vulkan::query {
struct Pool;
}

namespace gearoenix::vulkan::mesh {
struct Accel;
struct Manager final {
    constexpr static const int max_attempts = 10;
    GX_GET_RRF_PRV(engine::Engine, e)
    GX_GET_CVAL_PRV(bool, use_accel)

private:
    GX_CREATE_GUARD(blass_info)
    std::vector<std::tuple<
        VkAccelerationStructureGeometryKHR,
        VkAccelerationStructureBuildRangeInfoKHR,
        VkAccelerationStructureBuildGeometryInfoKHR,
        std::shared_ptr<Accel>,
        std::shared_ptr<query::Pool>,
        std::shared_ptr<buffer::Buffer>,
        core::sync::EndCaller<render::mesh::Mesh>>>
        blass_info;

    GX_CREATE_GUARD(pending_accel_meshes)
    std::vector<std::tuple<
        std::string,
        std::vector<math::BasicVertex>,
        std::vector<std::uint32_t>,
        core::sync::EndCaller<render::mesh::Mesh>,
        int>>
        pending_accel_meshes;
    void create_pending_accels() noexcept;
    void create_accel(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c,
        int attempts = 0) noexcept;
    void create_raster(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;
    void update_accel() noexcept;
    void update_raster() noexcept;

public:
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept;
    void create(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;
    void update() noexcept;
};
}

#endif
#endif
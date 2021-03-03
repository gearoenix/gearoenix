#ifndef GEAROENIX_VULKAN_MESH_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::vulkan::command {
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

namespace gearoenix::vulkan::sync {
struct Fence;
}

namespace gearoenix::vulkan::mesh {
struct Accel;
struct Manager final {
    GX_GET_RRF_PRV(engine::Engine, e)
    GX_GET_CVAL_PRV(bool, use_accel)

private:
    std::unique_ptr<core::sync::WorkWaiter> accel_creator;
    std::unique_ptr<core::sync::WorkWaiter> accel_creation_waiter;

    void create_accel(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;
    void create_accel_after_vertices_ready(
        std::string name,
        std::size_t vertices_count,
        std::size_t indices_count,
        core::sync::EndCaller<render::mesh::Mesh> c,
        std::shared_ptr<Accel> result) noexcept;
    void create_accel_after_blas_ready(
        std::shared_ptr<command::Buffer> cmd,
        std::shared_ptr<sync::Fence> fence,
        core::sync::EndCaller<render::mesh::Mesh> c,
        std::shared_ptr<Accel> result,
        std::shared_ptr<query::Pool> query_pool) noexcept;
    void create_accel_after_query_ready(
        std::shared_ptr<command::Buffer> cmd,
        std::shared_ptr<sync::Fence> fence,
        core::sync::EndCaller<render::mesh::Mesh> c,
        std::shared_ptr<Accel> result,
        std::shared_ptr<query::Pool> query_pool) noexcept;

    void create_raster(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;

public:
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept;
    void create(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;
};
}

#endif
#endif
#ifndef GEAROENIX_VULKAN_MESH_ACCEL_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_MANAGER_HPP
#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED

namespace gearoenix::vulkan::mesh {
struct Accel;
struct AccelManager final : public Manager {
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
    void create_accel_after_blas_copy(
        core::sync::EndCaller<render::mesh::Mesh> c,
        std::shared_ptr<Accel> result) noexcept;

public:
    explicit AccelManager(engine::Engine& e) noexcept;
    ~AccelManager() noexcept final;
    void create(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept final;
    void update() noexcept final;
};
}

#endif
#endif
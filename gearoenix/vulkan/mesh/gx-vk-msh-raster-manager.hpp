#ifndef GEAROENIX_VULKAN_MESH_RASTER_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_RASTER_MANAGER_HPP
#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED

namespace gearoenix::vulkan::mesh {
struct RasterManager final : public Manager {
private:
    void create_raster(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;

public:
    explicit RasterManager(engine::Engine& e) noexcept;
    ~RasterManager() noexcept final;
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
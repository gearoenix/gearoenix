#ifndef GEAROENIX_VULKAN_MESH_RASTER_HPP
#define GEAROENIX_VULKAN_MESH_RASTER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::mesh {
struct Raster final : public render::mesh::Mesh {
    GX_GET_CREF_PRV(std::shared_ptr<buffer::Buffer>, vertex)
    GX_GET_CREF_PRV(std::shared_ptr<buffer::Buffer>, index)

private:
    std::weak_ptr<Raster> self;

    Raster(std::shared_ptr<buffer::Buffer> vertex, std::shared_ptr<buffer::Buffer> index) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Raster> construct(
        engine::Engine&,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        const core::sync::EndCallerIgnored& c) noexcept;
    ~Raster() noexcept final;
    void set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept final;
};
}

#endif
#endif
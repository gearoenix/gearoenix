#ifndef GEAROENIX_VULKAN_MESH_RASTER_HPP
#define GEAROENIX_VULKAN_MESH_RASTER_HPP
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
struct Raster final : public render::mesh::Mesh {
    GX_GET_CREF_PRV(buffer::Buffer, vertex)
    GX_GET_CREF_PRV(buffer::Buffer, index)

private:
    std::weak_ptr<Raster> self;

    Raster(engine::Engine&, std::vector<math::BasicVertex> vertices, std::vector<std::uint32_t> indices) noexcept;

public:
    Raster(Raster&&) = delete;
    Raster(const Raster&) = delete;
    Raster& operator=(Raster&&) = delete;
    Raster& operator=(const Raster&) = delete;
    [[nodiscard]] static std::shared_ptr<Raster> construct(
        engine::Engine&, std::vector<math::BasicVertex> vertices, std::vector<std::uint32_t> indices) noexcept;
    ~Raster() noexcept final;
    void set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept final;
    [[nodiscard]] std::pair<VkDeviceAddress, VkDeviceAddress> get_buffers_address() const noexcept;
};
}

#endif
#endif
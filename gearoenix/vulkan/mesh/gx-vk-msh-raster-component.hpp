#ifndef GEAROENIX_VULKAN_MESH_RASTER_COMPONENT_HPP
#define GEAROENIX_VULKAN_MESH_RASTER_COMPONENT_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>

namespace gearoenix::vulkan::mesh {
struct Raster;
struct RasterComponent final : public core::ecs::Component {
    GX_GET_CREF_PRV(std::shared_ptr<Raster>, raster_mesh)

    explicit RasterComponent(std::shared_ptr<Raster> raster_mesh) noexcept;
    RasterComponent(RasterComponent&&) noexcept;
    RasterComponent(const RasterComponent&) = delete;
    RasterComponent& operator=(RasterComponent&&) = delete;
    RasterComponent& operator=(const RasterComponent&) = delete;
    ~RasterComponent() noexcept final;
};
}

#endif
#endif
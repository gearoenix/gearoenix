#ifndef GEAROENIX_VULKAN_MESH_RASTER_COMPONENT_HPP
#define GEAROENIX_VULKAN_MESH_RASTER_COMPONENT_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>

namespace gearoenix::vulkan::mesh {
struct Accel;
struct AccelComponent final : public core::ecs::Component {
    GX_GET_CREF_PRV(std::shared_ptr<Accel>, raster_mesh)

    explicit AccelComponent(std::shared_ptr<Accel> raster_mesh) noexcept;
    AccelComponent(AccelComponent&&) noexcept;
    AccelComponent(const AccelComponent&) = delete;
    AccelComponent& operator=(AccelComponent&&) = delete;
    AccelComponent& operator=(const AccelComponent&) = delete;
    ~AccelComponent() noexcept final;
};
}

#endif
#endif
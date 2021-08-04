#ifndef GEAROENIX_VULKAN_MESH_ACCEL_BUILDER_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_BUILDER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/mesh/gx-rnd-msh-builder.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::mesh {
struct Accel;
struct AccelBuilder final : public render::mesh::Builder {
    GX_GET_REFC_PRV(std::shared_ptr<Accel>, accel)

private:
    std::weak_ptr<AccelBuilder> self;

    AccelBuilder(
        engine::Engine&,
        const std::string& name,
        const std::vector<render::PbrVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        math::Aabb3&& occlusion_box,
        std::shared_ptr<Accel>&& accel) noexcept;

    void set_material_type_index(const std::type_index& material_type) noexcept final;

public:
    [[nodiscard]] static std::shared_ptr<AccelBuilder> construct(
        engine::Engine&,
        const std::string& name,
        const std::vector<render::PbrVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        math::Aabb3&& occlusion_box,
        core::sync::EndCaller<AccelBuilder>& c) noexcept;
    ~AccelBuilder() noexcept final;
};
}

#endif
#endif
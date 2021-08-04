#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-translucency-mode.hpp"

namespace gearoenix::core::ecs {
struct World;
}

namespace gearoenix::render::mesh {
struct Builder;
struct Mesh final : public core::ecs::Component {
    const math::Aabb3 original_box;
    /// It is going to be used in BVH creation, picker and ...
    math::Aabb3 updated_box;
    std::type_index material_type_index;
    TranslucencyMode translucency = TranslucencyMode::Opaque;
    bool is_enabled_rendering = true;
    bool is_shadow_caster = true;
    bool is_shadow_receiver = true;

    Mesh(
        const math::Aabb3& original_box,
        const std::type_index material_type_index,
        const TranslucencyMode translucency = TranslucencyMode::Opaque,
        const bool is_enabled_rendering = true,
        const bool is_shadow_caster = true,
        const bool is_shadow_receiver = true) noexcept
        : core::ecs::Component(this)
        , original_box(original_box)
        , updated_box(original_box)
        , material_type_index(material_type_index)
        , translucency(translucency)
        , is_enabled_rendering(is_enabled_rendering)
        , is_shadow_caster(is_shadow_caster)
        , is_shadow_receiver(is_shadow_receiver)
    {
    }

    Mesh(Mesh&&) noexcept = default;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
};
}
#endif
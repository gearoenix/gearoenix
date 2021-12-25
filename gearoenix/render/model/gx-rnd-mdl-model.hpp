#ifndef GEAROENIX_RENDER_MODEL_MODEL_HPP
#define GEAROENIX_RENDER_MODEL_MODEL_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-translucency-mode.hpp"
#include "../texture/gx-rnd-txt-binding-point.hpp"
#include <map>

namespace gearoenix::render::texture {
struct Texture;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Model final : public core::ecs::Component {
    const math::Aabb3 original_box; // Cached for better performance
    math::Aabb3 transformed_box; // It is going to be used in BVH creation, picker and ...
    std::type_index material_type_index;
    TranslucencyMode translucency = TranslucencyMode::Opaque;
    bool is_enabled_rendering = true;
    bool is_shadow_caster = true;
    bool is_shadow_receiver = true;
    std::map<texture::BindingPoint, std::shared_ptr<texture::Texture>> bound_textures; // This is mostly for keeping them alive
    const std::shared_ptr<mesh::Mesh> bound_mesh;
    std::uint64_t camera_visibility_flag = static_cast<std::uint64_t>(-1);

    Model(
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        std::type_index material_type_index,
        TranslucencyMode translucency = TranslucencyMode::Opaque,
        bool is_enabled_rendering = true,
        bool is_shadow_caster = true,
        bool is_shadow_receiver = true) noexcept;
    ~Model() noexcept final;
    Model(Model&&) noexcept;
};
}
#endif

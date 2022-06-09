#ifndef GEAROENIX_RENDER_MODEL_MODEL_HPP
#define GEAROENIX_RENDER_MODEL_MODEL_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-translucency-mode.hpp"
#include "../texture/gx-rnd-txt-binding-point.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <map>
#include <vector>

namespace gearoenix::render::texture {
struct Texture;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Model final : public core::ecs::Component {
    std::type_index material_type_index;
    TranslucencyMode translucency = TranslucencyMode::Opaque;
    bool is_shadow_caster = true;
    bool is_shadow_receiver = true;
    bool is_transformable = false;
    boost::container::flat_map<texture::BindingPoint, std::shared_ptr<texture::Texture>> bound_textures;
    const std::shared_ptr<mesh::Mesh> bound_mesh;
    boost::container::flat_set<core::ecs::Entity::id_t> blocked_cameras;
    std::uint64_t block_cameras_flags = static_cast<std::uint64_t>(-1);
    core::ecs::Entity::id_t scene_id = 0;

    Model(
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        std::type_index material_type_index,
        bool is_transformable,
        TranslucencyMode translucency = TranslucencyMode::Opaque,
        bool is_shadow_caster = true,
        bool is_shadow_receiver = true) noexcept;
    ~Model() noexcept final;
    Model(Model&&) noexcept;
};
}
#endif

#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "gx-rnd-sky-types.hpp"

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::skybox {
struct Skybox : core::ecs::Component {
    constexpr static core::ecs::component_index_t TYPE_INDEX = 16;
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, bound_mesh);
    GX_GET_CREF_PRT(Texture, bound_texture);
    GX_GETSET_VAL_PRT(core::ecs::entity_id_t, scene_id, core::ecs::invalid_entity_id);
    GX_GETSET_VAL_PRT(double, layer, 0.0);

    Skybox(
        core::ecs::component_index_t final_component_type_index,
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        Texture&& bound_texture,
        std::string&& name,
        core::ecs::entity_id_t entity_id);

public:
    ~Skybox() override;
    [[nodiscard]] bool is_equirectangular() const { return bound_texture.index() == 0; }
    [[nodiscard]] bool is_cube() const { return bound_texture.index() == 1; }
};
}

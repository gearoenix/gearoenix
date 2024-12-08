#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Probe : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 1024;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 28;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_VAL_PRT(std::uint64_t, radiance_mips_count, 0);
    GX_GET_CREF_PRT(math::Aabb3<double>, include_box);
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, irradiance);
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, radiance);
    GX_GET_VAL_PRT(core::ecs::entity_id_t, scene_id, core::ecs::invalid_entity_id);
    GX_GET_RRF_PRT(engine::Engine, e);

    Probe(
        engine::Engine& e,
        core::ecs::component_index_t final_component_type_index,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    Probe(
        engine::Engine& e,
        core::ecs::component_index_t final_component_type_index,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        core::ecs::entity_id_t entity_id);

public:
    ~Probe() override;

    virtual void set_scene_id(core::ecs::entity_id_t id);
    virtual void set_radiance(std::shared_ptr<texture::TextureCube>&&);
};
}
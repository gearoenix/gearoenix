#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../math/gx-math-vector-3d.hpp"

namespace gearoenix::render::light {
struct Light : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 0;
    constexpr static TypeIndex TYPE_INDEX = 12;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES {};
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES {};

    math::Vec3<float> colour = { 1.0f, 1.0f, 1.0f };
    core::ecs::entity_id_t scene_id = core::ecs::INVALID_ENTITY_ID;
    bool is_shadow_caster = false;

protected:
    Light(TypeIndex final_type_index, std::string&& name, core::ecs::entity_id_t entity_id);

public:
    ~Light() override;
    void show_debug_gui(const engine::Engine&) override;
};
}
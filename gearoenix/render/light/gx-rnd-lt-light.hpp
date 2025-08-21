#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-vector-3d.hpp"

namespace gearoenix::render::light {
struct Light : core::ecs::Component {
    constexpr static auto max_count = 32;
    constexpr static auto object_type_index = gearoenix_render_light_type_index;

    math::Vec3<float> colour = { 1.0f, 1.0f, 1.0f };
    core::ecs::Entity* scene = nullptr;
    bool is_shadow_caster = false;

protected:
    Light(core::ecs::Entity* entity, core::object_type_index_t final_type_index, std::string&& name);

public:
    ~Light() override;
    void show_debug_gui() override;
};
}
#ifndef GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../math/gx-math-vector-3d.hpp"

namespace gearoenix::render::light {
struct Light : core::ecs::Component {
    math::Vec3<float> colour = { 1.0f, 1.0f, 1.0f };
    core::ecs::entity_id_t scene_id = 0;
    bool is_shadow_caster = false;

protected:
    Light(std::type_index final_type_index, std::string&& name, core::ecs::entity_id_t entity_id);

public:
    ~Light() override;
    void show_debug_gui(const core::ecs::World&) override;
};
}
#endif

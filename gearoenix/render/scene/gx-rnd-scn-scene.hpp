#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../record/gx-rnd-rcd-scene.hpp"

namespace gearoenix::render::scene {
struct Scene : core::ecs::Component {
    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_render_scene_type_index;

    // radius, normal-jitter, min and max depth values for occlusion
    GX_GET_REF_PRT(math::Vec4<float>, ssao_settings);
    GX_GETSET_VAL_PRT(double, layer, 0.0);
    GX_GET_REF_PRT(record::Scene, record);
    GX_GET_REFC_PRT(std::string, shadow_render_pass_name);
    GX_GET_REFC_PRT(std::string, shadow_reflection_probe_render_pass_name);

    Scene(core::object_type_index_t final_type_index, double layer, std::string&& name);

public:
    ~Scene() override;
    virtual void update();
};
}

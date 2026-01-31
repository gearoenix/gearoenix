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
    GX_GET_REFC_PRT(std::string, forward_render_pass_name);
    GX_GET_REFC_PRT(std::string, shadow_render_pass_name);
    GX_GET_REFC_PRT(std::string, shadow_reflection_probe_render_pass_name);

    Scene(core::ecs::Entity* entity, core::object_type_index_t final_type_index, double layer, std::string&& name);
    Scene(core::object_type_index_t final_type_index, core::object_id_t id, std::string&& name);

    static void read(std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end);
    void write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end_caller) override;

public:
    ~Scene() override;
    virtual void update();
};
}

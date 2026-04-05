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
    GX_GET_REF_PRT(record::Scene, record);
    GX_GET_REFC_PRT(std::string, forward_render_pass_name);
    GX_GET_REFC_PRT(std::string, shadow_render_pass_name);
    GX_GET_REFC_PRT(std::string, shadow_reflection_probe_render_pass_name);
    GX_GETSET_VAL_PRT(core::fp_t, layer, 0.0);
    GX_GETSET_VAL_PRT(bool, changed, true);

    Scene(core::ecs::Entity* entity, core::object_type_index_t final_type_index, core::fp_t layer, std::string&& name);
    Scene(core::object_type_index_t final_type_index, core::object_id_t id, std::string&& name);

    static void read(std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end);
    void write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end_caller) override;

public:
    ~Scene() override;
    virtual void update_per_frame();

    /// Update the whole scene after major changes
    ///
    /// This function updates the internal structure of the scene after any major change in the scene.
    /// After these changes you need to call this function:
    ///   * - Any child-entity addition to or deletion from a scene.
    ///   * - Any transformation in static entities (not the dynamic entities).
    ///   * - Any update in enable/disable of any child-entity.
    ///   * - Look into the implementation for further info and update this list.
    /// @note This is an expensive operation, try to avoid calling it too often and try to batch your changes.
    virtual void update_after_change();
};
}

#include "gx-vk-mdl-model.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../../render/record/gx-rnd-rcd-light.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-draw-state.hpp"
#include "../light/gx-vk-lt-shadow-caster-directional.hpp"
#include "../material/gx-vk-mat-material.hpp"
#include "../mesh/gx-vk-msh-mesh.hpp"
#include "../pipeline/gx-vk-pip-format-pipelines.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../reflection/gx-vk-rfl-baked.hpp"

gearoenix::vulkan::model::Model::Model(core::ecs::Entity* entity, render::model::meshes_set_t&& ms, std::string&& name, const bool is_transformable, const bool is_skinned)
    : render::model::Model(entity, core::ecs::ComponentType::create_index(this), is_transformable, std::move(ms), std::move(name), is_skinned)
{
    for (const auto& mesh : meshes) {
        const auto* const m = core::cast_ptr<mesh::Mesh>(mesh.get());
        GX_ASSERT_D(m);
        auto& dc = draw_caches.emplace_back();
        m->set(is_skinned, dc);
    }
}

gearoenix::vulkan::model::Model::~Model() = default;

void gearoenix::vulkan::model::Model::render_shadow(DrawState& draw_state)
{
    draw_state.push_constants.model_index = shader_data_index;
    GX_ASSERT_D(is_skinned == draw_state.camera_model->model->bones_count > 0);
    for (const auto& dc : draw_caches) {
        draw_mesh(draw_state, dc, dc.material_draw_cache.shadow_pipeline_index);
    }
}

void gearoenix::vulkan::model::Model::render_forward(DrawState& draw_state)
{
    draw_state.push_constants.model_index = shader_data_index;
    GX_ASSERT_D(is_skinned == draw_state.camera_model->model->bones_count > 0);
    for (const auto& dc : draw_caches) {
        draw_mesh(draw_state, dc, dc.material_draw_cache.forward_pipeline_index);
    }
}

void gearoenix::vulkan::model::Model::after_record(const std::uint64_t frame_number, const render::record::CameraModel& rec_cam_mdl)
{
    if (last_update_frame == frame_number) {
        return;
    }
    last_update_frame = frame_number;

    const auto& rec_mdl = *rec_cam_mdl.model;
    const auto& trn = *rec_mdl.transform;

    auto shader_data = descriptor::UniformIndexer<GxShaderDataModel>::get().get_next();
    shader_data_index = shader_data.get_index();
    auto& sd = *shader_data.get_ptr();

    sd.m = trn.get_global_matrix().to<float>();
    sd.inv_transpose_m = trn.get_transposed_inverted_global_matrix().to<float>();

    if (rec_mdl.armature) {
        auto& bone_indexer = descriptor::UniformIndexer<GxShaderDataBone>::get();
        sd.bones_begin_index = bone_indexer.get_current_index();
        for (const auto& all_bones = rec_mdl.armature->get_all_bones(); const auto* const b : all_bones) {
            auto bone_sd = bone_indexer.get_next();
            auto& [m, inv_transpose_m] = *bone_sd.get_ptr();
            m = b->get_global_matrix().to<float>();
            inv_transpose_m = b->get_transposed_inverted_global_matrix().to<float>();
        }
        sd.bones_end_index = bone_indexer.get_current_index();
    } else {
        sd.bones_begin_index = 0;
        sd.bones_end_index = 0;
    }

    const auto& lights = rec_mdl.lights;

    sd.point_light_begin_index = 0;
    sd.point_light_end_index = 0;

    sd.directional_light_begin_index = 0;
    sd.directional_light_end_index = 0;

    sd.shadow_caster_directional_lights_count = static_cast<std::uint32_t>(lights.shadow_caster_directionals.size());
    for (std::uint32_t li = 0; li < sd.shadow_caster_directional_lights_count; ++li) {
        auto& rl = *lights.shadow_caster_directionals[li];
        auto& l = *core::cast_ptr<light::ShadowCasterDirectional>(rl.shadow_caster_directional);
        l.after_record(frame_number, rl);
        sd.shadow_caster_directional_lights[li] = l.get_shader_index();
    }

    auto& probe = *dynamic_cast<reflection::Probe*>(rec_mdl.probe);
    probe.after_record(frame_number);

    sd.reflection_probe_index = probe.get_shader_data_index();
}
#endif

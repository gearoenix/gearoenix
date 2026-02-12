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
#include "../material/gx-vk-mat-material.hpp"
#include "../mesh/gx-vk-msh-mesh.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../light/gx-vk-lt-shadow-caster-directional.hpp"

gearoenix::vulkan::model::Model::Model(core::ecs::Entity* entity, render::model::meshes_set_t&& ms, std::string&& name, const bool is_transformable)
    : render::model::Model(entity, core::ecs::ComponentType::create_index(this), is_transformable, std::move(ms), std::move(name))
{
    for (const auto& mesh : meshes) {
        auto m = core::cast_shared<mesh::Mesh>(std::shared_ptr(mesh));
        GX_ASSERT_D(m);
        gapi_meshes.insert(std::move(m));
    }
}

gearoenix::vulkan::model::Model::~Model() = default;

void gearoenix::vulkan::model::Model::render_shadow(
    const render::record::CameraModel& camera_model, const VkCommandBuffer cmd,
    pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline)
{
    pc.model_index = shader_data_index;
    const auto skinned = camera_model.model->bones_count > 0;
    for (const auto& msh : gapi_meshes) {
        msh->get_gapi_material()->bind_shadow(cmd, skinned, pc, current_bound_pipeline);
        msh->draw(cmd, pc);
    }
}

void gearoenix::vulkan::model::Model::render_forward(
    const render::record::CameraModel& camera_model, const VkCommandBuffer cmd,
    pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline)
{
    pc.model_index = shader_data_index;
    const auto skinned = camera_model.model->bones_count > 0;
    for (const auto& msh : gapi_meshes) {
        msh->get_gapi_material()->bind_forward(cmd, skinned, pc, current_bound_pipeline);
        msh->draw(cmd, pc);
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

    sd.m = math::Mat4x4<float>(trn.get_global_matrix());
    sd.inv_transpose_m = math::Mat4x4<float>(trn.get_transposed_inverted_global_matrix());

    if (rec_mdl.armature) {
        auto& bone_indexer = descriptor::UniformIndexer<GxShaderDataBone>::get();
        sd.bones_begin_index = bone_indexer.get_current_index();
        for (const auto& all_bones = rec_mdl.armature->get_all_bones(); const auto* const b : all_bones) {
            auto bone_sd = bone_indexer.get_next();
            auto& [m, inv_transpose_m] = *bone_sd.get_ptr();
            m = math::Mat4x4<float>(b->get_global_matrix());
            inv_transpose_m = math::Mat4x4<float>(b->get_transposed_inverted_global_matrix());
        }
        sd.bones_end_index = bone_indexer.get_current_index();
        if (sd.bones_begin_index != sd.bones_end_index) {
            ++sd.bones_begin_index;
        }
    } else {
        sd.bones_begin_index = 0;
        sd.bones_end_index = 0;
    }

    const auto& lights = rec_mdl.lights;

    sd.point_light_begin_index = 0;
    sd.point_light_end_index = 0;

    sd.directional_light_begin_index = 0;
    sd.directional_light_end_index = 0;

    sd.shadow_caster_directional_lights_count = lights.shadow_caster_directionals.size();
    for(std::uint32_t li = 0; li < sd.shadow_caster_directional_lights_count; ++li) {
        auto& rl = *lights.shadow_caster_directionals[li];
        auto& l = *core::cast_ptr<light::ShadowCasterDirectional>(rl.shadow_caster_directional);
        l.after_record(frame_number, rl);
        sd.shadow_caster_directional_lights[li] = l.get_shader_index();
    }

    sd.reflection_probe_index = 0;
}
#endif

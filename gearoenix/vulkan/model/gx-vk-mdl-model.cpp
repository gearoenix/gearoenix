#include "gx-vk-mdl-model.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../material/gx-vk-mat-manager.hpp"
#include "../mesh/gx-vk-msh-mesh.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"

gearoenix::vulkan::model::Model::Model(core::ecs::Entity* entity, render::model::meshes_set_t&& ms, std::string&& name, bool is_transformable)
    : render::model::Model(entity, core::ecs::ComponentType::create_index(this), is_transformable, std::move(ms), std::move(name))
{
    for (const auto& mesh : meshes) {
        auto m = core::cast_shared<mesh::Mesh>(std::shared_ptr(mesh));
        GX_ASSERT_D(m);
        gapi_meshes.insert(std::move(m));
    }
}

gearoenix::vulkan::model::Model::~Model() = default;

void gearoenix::vulkan::model::Model::render_shadow(const render::record::Camera& camera, const render::record::CameraModel& camera_model, const VkCommandBuffer cmd)
{
    for (const auto& msh : gapi_meshes) {
        msh->get_gapi_material()->bind_shadow(cmd);
        msh->draw(*msh, camera, camera_model, cmd);
    }
}

void gearoenix::vulkan::model::Model::render_forward(const scene::Scene& scene, const render::record::Camera& camera, const render::record::CameraModel& camera_model, const VkCommandBuffer cmd)
{
    for (const auto& msh : gapi_meshes) {
        msh->get_gapi_material()->bind_forward(cmd);
        msh->draw(*msh, camera, camera_model, cmd);
    }
}

void gearoenix::vulkan::model::Model::after_record(const render::record::CameraModel& rec_cam_mdl)
{
    const auto& rec_mdl = *rec_cam_mdl.model;
    const auto& trn = *rec_mdl.transform;

    const auto [ptr, index] = descriptor::UniformIndexer<GxShaderDataModel>::get().get_next();
    shader_data_index = index;

    // Model matrices
    ptr->m = math::Mat4x4<float>(trn.get_global_matrix());
    ptr->inv_transpose_m = math::Mat4x4<float>(trn.get_transposed_inverted_global_matrix());

    auto& bone_indexer = descriptor::UniformIndexer<GxShaderDataBone>::get();
    ptr->bones_begin_index = bone_indexer.get_current_index();
    for (const auto& all_bones = rec_mdl.armature->get_all_bones(); const auto* const b : all_bones) {
        const auto [bone_ptr, _] = bone_indexer.get_next();
        bone_ptr->m = math::Mat4x4<float>(b->get_global_matrix());
        bone_ptr->inv_transpose_m = math::Mat4x4<float>(b->get_transposed_inverted_global_matrix());
    }
    ptr->bones_end_index = bone_indexer.get_current_index();
    if (ptr->bones_begin_index != ptr->bones_end_index) {
        ++ptr->bones_begin_index;
    }

    ptr->point_light_begin_index = 0;
    ptr->point_light_end_index = 0;

    ptr->directional_light_begin_index = 0;
    ptr->directional_light_end_index = 0;

    ptr->shadow_caster_directional_light_begin_index = 0;
    ptr->shadow_caster_directional_light_end_index = 0;

    ptr->reflection_probe_index = 0;
}
#endif

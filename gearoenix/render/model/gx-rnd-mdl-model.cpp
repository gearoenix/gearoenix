#include "gx-rnd-mdl-model.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../camera/gx-rnd-cmr-uniform.hpp"
#include "../imgui/gx-rnd-imgui-type-table.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"
#include "../material/gx-rnd-mat-material.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "gx-rnd-mdl-uniform.hpp"

gearoenix::render::model::Model::Model(
    core::ecs::Entity* const entity,
    const core::object_type_index_t final_component_type,
    const bool is_transformable,
    std::shared_ptr<physics::Transformation>&& transformation,
    meshes_set_t&& meshes,
    std::string&& name,
    std::shared_ptr<physics::animation::Armature>&& armature)
    : Component(entity, final_component_type, std::move(name))
    , uniform(buffer::Manager::get_range(buffer::UniformRegionIndex::models))
    , transform(std::move(transformation))
    , is_transformable(is_transformable)
    , meshes(std::move(meshes))
    , armature(std::move(armature))
{
    create_bones_uniforms();
}

gearoenix::render::model::Model::~Model() = default;

void gearoenix::render::model::Model::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        Component::show_debug_gui();

        imgui::table_scope("##gearoenix::render::model::Model", [this] {
            ImGui::Text("Transformable:");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##is_transformable", &is_transformable);
        });

        if (!meshes.empty() && ImGui::TreeNode("Meshes")) {
            for (const auto& msh : meshes) {
                msh->show_debug_gui();
            }
            ImGui::TreePop();
        }
    });
}

void gearoenix::render::model::Model::update_uniform()
{
    auto& ref = uniform.get_ref<GxShaderDataModel>();
    ref.m = transform->get_global_matrix();
    ref.inv_transpose_m = transform->get_transposed_inverted_global_matrix().to<float>();
    if (!armature) {
        GX_ASSERT_D(!bones_uniform.has_value());
        ref.bones_point_lights_directional_lights_shadow_caster_directional_lights_count.x = 0;
    } else {
        GX_ASSERT_D(bones_uniform.has_value());
        const auto& bones = armature->get_all_bones();
        ref.bones_point_lights_directional_lights_shadow_caster_directional_lights_count.x = static_cast<std::uint32_t>(bones.size());
        ref.reflection_probe_bones_begin_reserved_reserved.y = bones_uniform->shader_data_index;
        GX_ASSERT(bones.size() * sizeof(GxShaderDataBone) <= bones_uniform->size);
        auto* const bup = &bones_uniform->get_ref<GxShaderDataBone>();
        for (std::size_t bone_i = 0; bone_i < bones.size(); ++bone_i) {
            bones[bone_i]->update_uniform(bup[bone_i]);
        }
    }
}

void gearoenix::render::model::Model::create_bones_uniforms()
{
    bones_uniform.reset();
    if (!armature) {
        return;
    }
    const auto& bones = armature->get_all_bones();
    auto uni_args = buffer::Manager::get_bones_range(static_cast<std::int64_t>(bones.size()));
    bones_uniform.emplace(std::move(uni_args));
}

void gearoenix::render::model::Model::update_bones_uniform(const math::Mat4x4<float>& view_projection, GxShaderDataCameraJointModel* const mvp_uniform_arr_ptr)
{
    if (!bones_uniform) {
        return;
    }
    const auto bones_count = armature->get_all_bones().size();
    const auto* const bup = &bones_uniform->get_ref<GxShaderDataBone>();
    for (std::size_t bone_i = 0; bone_i < bones_count; ++bone_i) {
        mvp_uniform_arr_ptr[bone_i].mvp = view_projection * bup[bone_i].m;
    }
}

bool gearoenix::render::model::Model::has_transparent_material() const
{
    for (const auto& msh : meshes) {
        if (msh->get_bound_material()->get_transparency() == material::Transparency::Transparent) {
            return true;
        }
    }
    return false;
}

bool gearoenix::render::model::Model::needs_mvp() const
{
    for (const auto& msh : meshes) {
        if (msh->get_bound_material()->get_need_model_view_projection_matrix()) {
            return true;
        }
    }
    return false;
}

std::uint32_t gearoenix::render::model::Model::get_bones_count() const
{
    return armature ? static_cast<std::uint32_t>(armature->get_all_bones().size()) : 0;
}

const GxShaderDataModel& gearoenix::render::model::Model::get_uniform_inner_pointer() const
{
    return uniform.get_ref<GxShaderDataModel>();
}

GxShaderDataModel& gearoenix::render::model::Model::get_uniform_inner_pointer()
{
    return uniform.get_ref<GxShaderDataModel>();
}
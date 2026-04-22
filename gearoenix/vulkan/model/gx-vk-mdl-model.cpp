#include "gx-vk-mdl-model.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/model/gx-rnd-mdl-uniform.hpp"
#include "../../render/record/gx-rnd-rcd-light.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-draw-state.hpp"
#include "../material/gx-vk-mat-material.hpp"
#include "../mesh/gx-vk-msh-mesh.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"

gearoenix::vulkan::model::Model::Model(
    core::ecs::Entity* const entity,
    const bool is_transformable,
    std::shared_ptr<physics::Transformation>&& transformation,
    render::model::meshes_set_t&& in_meshes,
    std::string&& name,
    std::shared_ptr<physics::animation::Armature>&& arm)
    : render::model::Model(entity, core::ecs::ComponentType::create_index(this), is_transformable, std::move(transformation), std::move(in_meshes), std::move(name), std::move(arm))
{
    for (const auto& mesh : meshes) {
        const auto* const m = core::cast_ptr<mesh::Mesh>(mesh.get());
        GX_ASSERT_D(m);
        auto& dc = draw_caches.emplace_back();
        m->set(nullptr != armature, dc);
    }
}

gearoenix::vulkan::model::Model::~Model() = default;

void gearoenix::vulkan::model::Model::render_shadow(DrawState& draw_state)
{
    draw_state.push_constants.model_index = uniform.shader_data_index;
    for (const auto& dc : draw_caches) {
        draw_mesh(draw_state, dc, dc.material_draw_cache.shadow_pipeline_index);
    }
}

void gearoenix::vulkan::model::Model::render_forward(DrawState& draw_state)
{
    draw_state.push_constants.model_index = uniform.shader_data_index;
    for (const auto& dc : draw_caches) {
        draw_mesh(draw_state, dc, dc.material_draw_cache.forward_pipeline_index);
    }
}
#endif

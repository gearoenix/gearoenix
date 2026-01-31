#include "gx-vk-mdl-model.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../mesh/gx-vk-msh-mesh.hpp"
#include "../material/gx-vk-mat-manager.hpp"

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
#endif

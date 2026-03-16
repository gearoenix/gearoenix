#include "gx-vk-mdl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "gx-vk-mdl-model.hpp"

gearoenix::vulkan::model::Manager::Manager()
    : Singleton<Manager>(this)
    , model_uniform_indexer(render::model::Model::max_count)
    , bone_uniform_indexer(physics::animation::Bone::max_count)
{
    core::ecs::ComponentType::add<Model>();
}

gearoenix::vulkan::model::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::model::Manager::build(std::string&& name, core::ecs::Entity* const parent, render::model::meshes_set_t&& meshes, const bool is_transformable)
{
    auto entity = render::model::Manager::build(std::move(name), parent, std::move(meshes), is_transformable);
    entity->add_component(core::Object::construct<Model>(entity.get(), std::move(meshes), std::move(name), is_transformable));
    return entity;
}

void gearoenix::vulkan::model::Manager::update()
{
    model_uniform_indexer.reset();
    bone_uniform_indexer.reset();
}

void gearoenix::vulkan::model::Manager::upload_uniforms()
{
    model_uniform_indexer.update();
    bone_uniform_indexer.update();
}

#endif

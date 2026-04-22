#include "gx-vk-mdl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "gx-vk-mdl-model.hpp"

gearoenix::vulkan::model::Manager::Manager()
    : Singleton<Manager>(this)
{
    core::ecs::ComponentType::add<Model>();
}

gearoenix::vulkan::model::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::model::Manager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    render::model::meshes_set_t&& meshes,
    const bool is_transformable,
    std::shared_ptr<physics::animation::Armature>&& armature)
{
    auto entity = render::model::Manager::build(std::string(name), parent, render::model::meshes_set_t(meshes), is_transformable, std::shared_ptr(armature));
    entity->add_component(core::Object::construct<Model>(
        entity.get(),
        is_transformable,
        entity->get_component_shared_ptr<physics::Transformation>(),
        std::move(meshes),
        std::move(name),
        std::move(armature)));
    return entity;
}

#endif

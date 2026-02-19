#include "gx-vk-sky-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-vk-sky-skybox.hpp"

gearoenix::vulkan::skybox::Manager::Manager()
    : Singleton<Manager>(this)
{
    core::ecs::ComponentType::add<Skybox>();
}

gearoenix::vulkan::skybox::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::skybox::Manager::build(
    std::string&& name, core::ecs::Entity* const parent, std::shared_ptr<render::texture::Texture>&& bound_texture, std::shared_ptr<render::mesh::Mesh>&& bound_mesh)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<Skybox>(entity.get(), std::move(bound_mesh), std::move(bound_texture), std::string(entity->get_object_name())));
    return entity;
}

#endif

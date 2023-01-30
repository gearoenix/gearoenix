#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-world.hpp"
#include <imgui.h>

gearoenix::core::ecs::Entity::Entity(
    Archetype* const archetype,
    unsigned char* const components,
    std::string&& name) noexcept
    : archetype(archetype)
    , components(components)
    , name(std::move(name))
{
}

gearoenix::core::ecs::Entity::Entity(Entity&& o) noexcept
    : archetype(o.archetype)
    , components(o.components)
    , name(std::move(o.name))
{
}

std::atomic<gearoenix::core::ecs::entity_id_t> gearoenix::core::ecs::Entity::last_id(1028);

void gearoenix::core::ecs::Entity::show_debug_gui() noexcept
{
    if (ImGui::TreeNode(name.c_str())) {
        for (const auto& ci : archetype->components_indices) {
            reinterpret_cast<Component*>(components + ci.second)->show_debug_gui();
        }
        ImGui::TreePop();
    }
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(const entity_id_t id, std::string&& name, sync::EndCaller&& end_caller) noexcept
    : id(id)
    , name(std::move(name))
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(std::string&& name, sync::EndCaller&& end_caller) noexcept
    : id(++Entity::last_id)
    , name(std::move(name))
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(EntityBuilder&& o) noexcept
    : id(o.id)
    , name(std::move(o.name))
    , components(std::move(o.components))
    , end_caller(std::move(o.end_caller))
{
}

const gearoenix::core::ecs::Component* gearoenix::core::ecs::EntityBuilder::get_component(const std::type_index component_type) const noexcept
{
    const auto search = components.find(component_type);
    if (components.end() == search)
        return nullptr;
    return search->second.get();
}

gearoenix::core::ecs::Component* gearoenix::core::ecs::EntityBuilder::get_component(const std::type_index component_type) noexcept
{
    auto search = components.find(component_type);
    if (components.end() == search)
        return nullptr;
    return search->second.get();
}

gearoenix::core::ecs::EntitySharedBuilder::EntitySharedBuilder(World* const world, std::string&& name, sync::EndCaller&& end_caller) noexcept
    : world(world)
    , builder(std::move(name), std::move(end_caller))
{
}

gearoenix::core::ecs::EntitySharedBuilder::~EntitySharedBuilder() noexcept
{
    world->delayed_create_entity_with_builder(std::move(builder));
}

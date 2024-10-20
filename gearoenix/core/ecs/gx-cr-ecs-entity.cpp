#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-world.hpp"
#include <imgui.h>

namespace {
std::shared_ptr<gearoenix::core::ecs::Component> null_component(nullptr);
}

gearoenix::core::ecs::Entity::Entity(
    Archetype* const archetype,
    std::shared_ptr<Component>* const components,
    std::string&& name)
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
    o.archetype = nullptr;
    o.components = nullptr;
}

std::atomic<gearoenix::core::ecs::entity_id_t> gearoenix::core::ecs::Entity::last_id(1028);

gearoenix::core::ecs::Entity::~Entity() = default;

void gearoenix::core::ecs::Entity::show_debug_gui(const World& w) const
{
    static boost::container::flat_set<std::size_t> indices;
    if (ImGui::TreeNode(name.c_str())) {
        indices.clear();
        for (const auto& ci : archetype->components_indices) {
            indices.emplace(ci.second);
        }
        for (const auto i : indices) {
            components[i]->show_debug_gui(w);
        }
        ImGui::TreePop();
    }
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(const entity_id_t id, std::string&& name, job::EndCaller<>&& end_caller)
    : id(id)
    , name(std::move(name))
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(std::string&& name, job::EndCaller<>&& end_caller)
    : id(++Entity::last_id)
    , name(std::move(name))
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(EntityBuilder&& o) noexcept
    : id(o.id)
    , name(std::move(o.name))
    , components(std::move(o.components))
    , bases_to_leaves(std::move(o.bases_to_leaves))
    , end_caller(std::move(o.end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::~EntityBuilder() = default;

const std::shared_ptr<gearoenix::core::ecs::Component>& gearoenix::core::ecs::EntityBuilder::get_component(
    const std::type_index component_type) const
{
    const auto search = bases_to_leaves.find(component_type);
    if (bases_to_leaves.end() == search) {
        return null_component;
    }
    return get_component_final_type(search->second);
}

const std::shared_ptr<gearoenix::core::ecs::Component>& gearoenix::core::ecs::EntityBuilder::get_component_final_type(
    const std::type_index component_type) const
{
    const auto search = components.find(component_type);
    if (components.end() == search) {
        return null_component;
    }
    return search->second;
}

void gearoenix::core::ecs::EntityBuilder::add_component(std::shared_ptr<Component>&& component)
{
    auto c = std::make_pair(component->get_final_type_index(), std::move(component));
    if (const auto search = components.find(c.first); search != components.end()) {
        GX_LOG_F("Component '" << c.second->get_name() << "' already exists in entity '" << id);
    }
    for (const auto& base_types = c.second->get_hierarchy_types(); const auto& t : base_types) {
        if (const auto search = bases_to_leaves.find(t.first); search != bases_to_leaves.end()) {
            GX_LOG_F("Component '" << c.second->get_name() << "' already exists in entity components hierarchy '" << id);
        }
        bases_to_leaves.emplace(t.first, c.first);
    }
    components.emplace(std::move(c));
}

gearoenix::core::ecs::EntitySharedBuilder::EntitySharedBuilder(World* const world, std::string&& name, job::EndCaller<>&& end_caller)
    : builder(std::move(name), std::move(end_caller))
    , world(world)
{
}

gearoenix::core::ecs::EntitySharedBuilder::~EntitySharedBuilder()
{
    world->delayed_create_entity(std::move(builder));
}

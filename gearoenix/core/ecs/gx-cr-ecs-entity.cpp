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

void gearoenix::core::ecs::Entity::show_debug_gui(const render::engine::Engine& e) const
{
    if (!ImGui::TreeNode(name.c_str())) {
        return;
    }
    static boost::container::flat_set<std::uint32_t> indices;
    indices.clear();
    for (const auto& ci : archetype->components_indices) {
        indices.emplace(ci.second);
    }
    for (const auto i : indices) {
        components[i]->show_debug_gui(e);
    }
    ImGui::TreePop();
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
    , final_types(o.final_types)
    , all_types(o.all_types)
    , all_types_to_components(std::move(o.all_types_to_components))
    , end_caller(std::move(o.end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::~EntityBuilder() = default;

const std::shared_ptr<gearoenix::core::ecs::Component>& gearoenix::core::ecs::EntityBuilder::get_component(
    const Component::TypeIndex component_type) const
{
    const auto search = all_types_to_components.find(component_type);
    if (all_types_to_components.end() == search) {
        return null_component;
    }
    return search->second;
}

void gearoenix::core::ecs::EntityBuilder::add_component(std::shared_ptr<Component>&& component)
{
    const auto fti = component->get_final_type_index();
    auto c = std::make_pair(fti, std::move(component));
    if (all_types.contains(fti)) {
        GX_LOG_F("Component '" << c.second->get_name() << "' already exists in entity '" << id);
    }
    for (const auto& pts = Component::get_type_info(fti).get_all_parents(); const auto pt : pts) {
        if (all_types.contains(fti)) {
            GX_LOG_F("Component '" << c.second->get_name() << "' already exists in entity components hierarchy '" << id);
        }
        all_types.add(pt);
        all_types_to_components.emplace(pt, c.second);
    }
    all_types.add(fti);
    final_types.add(fti);
    all_types_to_components.emplace(std::move(c));
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

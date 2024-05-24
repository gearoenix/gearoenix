#include "gx-cr-ecs-world.hpp"
#include <boost/mp11/algorithm.hpp>
#include <imgui.h>

void gearoenix::core::ecs::World::create_entity(EntityBuilder&& b)
{
    auto* const a = get_archetype(b.components);
    auto* const cs = a->allocate_entity(b.id, b.components);
    entities.emplace(b.id, Entity(a, cs, std::string(b.name)));
    if (name_to_entity_id.contains(b.name))
        GX_LOG_F("Entity with name `" << b.name << "' is already in the world.");
    name_to_entity_id.emplace(std::move(b.name), b.id);
}

void gearoenix::core::ecs::World::delayed_create_entity(EntityBuilder&& b, job::EndCaller<>&& callback)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action {
        .callback = std::move(callback),
        .variant = Action::CreateEntity { .builder = std::move(b) },
    });
}

void gearoenix::core::ecs::World::delete_entity(const entity_id_t id)
{
    const auto search = entities.find(id);
#ifdef GX_DEBUG_MODE
    if (entities.end() == search) {
        GX_LOG_E("Deleting an already deleted entity: " << id);
        return;
    }
#endif
    const auto& e = search->second;
    e.archetype->delete_entity(e.components);
    name_to_entity_id.erase(e.name);
    entities.erase(search);
}

void gearoenix::core::ecs::World::delayed_delete_entity(const entity_id_t id, job::EndCaller<>&& callback)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action {
        .callback = std::move(callback),
        .variant = Action::DeleteEntity { .id = id } });
}

void gearoenix::core::ecs::World::add_components_map(const entity_id_t id, EntityBuilder::components_t&& c)
{
    if (c.empty()) {
        return;
    }
    auto* const entity = get_entity(id);
    if (nullptr == entity) {
        GX_LOG_E("Entity: " << id << " does not exist.");
        return;
    }
    const auto old_archetype = entity->archetype;
    const auto old_components = entity->components;
    old_archetype->move_out_entity(old_components, c);
    const auto new_archetype = get_archetype(c);
    entity->archetype = new_archetype;
    entity->components = new_archetype->allocate_entity(id, c);
}

void gearoenix::core::ecs::World::delayed_add_components_map(
    const entity_id_t ei, EntityBuilder::components_t&& cs, job::EndCaller<>&& callback)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action {
        .callback = std::move(callback),
        .variant = Action::AddComponents {
            .id = ei,
            .components = std::move(cs),
        },
    });
}

void gearoenix::core::ecs::World::remove_components_list(
    const entity_id_t id, const std::type_index* const ts, const std::size_t sz)
{
    if (0 == sz) {
        GX_LOG_E("At least one component must be selected to remove. Entity: " << id);
        return;
    }
    auto* const entity = get_entity(id);
    if (nullptr == entity) {
        GX_LOG_E("Entity " << id << " does not exist.");
        return;
    }
    auto* const old_archetype = entity->archetype;
    auto* const old_components = entity->components;
    EntityBuilder::components_t cs;
    cs.reserve(old_archetype->components_indices.size());
    old_archetype->move_out_entity(old_components, cs);
    for (std::size_t ti = 0; ti < sz; ++ti) {
        const auto& t = ts[ti];
        for (auto iter = cs.begin(); cs.end() != iter;) {
            if (iter->second->get_all_the_hierarchy_types_except_component().contains(t)) {
                iter = cs.erase(iter);
            } else {
                ++iter;
            }
        }
    }
    auto* const new_archetype = get_archetype(cs);
    entity->archetype = new_archetype;
    entity->components = new_archetype->allocate_entity(id, cs);
}

void gearoenix::core::ecs::World::delayed_remove_components_list(
    const entity_id_t ei,
    std::vector<std::type_index>&& cs,
    job::EndCaller<>&& callback)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action {
        .callback = std::move(callback),
        .variant = Action::DeleteComponents {
            .id = ei,
            .component_types = std::move(cs),
        },
    });
}

gearoenix::core::ecs::Archetype* gearoenix::core::ecs::World::get_archetype(const EntityBuilder::components_t& cs)
{
    thread_local Archetype::id_t archetype_id;
    archetype_id.clear();
    for (const auto& c : cs) {
        archetype_id.emplace(c.first);
    }
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        bool is_ok = false;
        std::tie(search, is_ok) = archetypes.emplace(archetype_id, std::unique_ptr<Archetype>(new Archetype(cs)));
        if (!is_ok)
            GX_LOG_F("Insertion in archetype map was not successful");
    }
    return search->second.get();
}

gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const entity_id_t id)
{
    const auto search = entities.find(id);
    if (entities.end() == search)
        return nullptr;
    return &search->second;
}

const gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const entity_id_t id) const
{
    const auto search = entities.find(id);
    if (entities.end() == search)
        return nullptr;
    return &search->second;
}

void gearoenix::core::ecs::World::update()
{
    decltype(delayed_actions) actions;
    {
        const std::lock_guard _lg(delayed_actions_lock);
        std::swap(actions, delayed_actions);
    }

    constexpr std::size_t create_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::CreateEntity>::value;
    constexpr std::size_t delete_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::DeleteEntity>::value;
    constexpr std::size_t add_components_index = boost::mp11::mp_find<decltype(Action::variant), Action::AddComponents>::value;
    constexpr std::size_t delete_components_index = boost::mp11::mp_find<decltype(Action::variant), Action::DeleteComponents>::value;

    for (auto& action : actions) {
        switch (action.variant.index()) {
        case create_entity_index:
            create_entity(std::move(std::get<create_entity_index>(std::move(action.variant)).builder));
            break;
        case delete_entity_index:
            delete_entity(std::get<delete_entity_index>(action.variant).id);
            break;
        case add_components_index: {
            auto&& [id, cs] = std::get<add_components_index>(std::move(action.variant));
            add_components_map(id, std::move(cs));
            break;
        }
        case delete_components_index: {
            auto&& [id, ct] = std::get<delete_components_index>(std::move(action.variant));
            remove_components_list(id, ct.data(), ct.size());
            break;
        }
        default:
            GX_UNEXPECTED;
        }
    }
}

void gearoenix::core::ecs::World::show_debug_gui()
{
    if (ImGui::TreeNode("ECS World")) {
        ImGui::Text("Number of Archetypes: %zu", archetypes.size());
        ImGui::Text("Number of Entities: %zu", entities.size());
        ImGui::Text("Number of Entity Names: %zu", name_to_entity_id.size());
        if (ImGui::TreeNode("Entities")) {
            for (const auto& name_id : name_to_entity_id) {
                auto search = entities.find(name_id.second);
                GX_ASSERT_D(entities.end() != search);
                search->second.show_debug_gui();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

std::shared_ptr<gearoenix::core::ecs::EntitySharedBuilder> gearoenix::core::ecs::World::create_shared_builder(std::string&& name, job::EndCaller<>&& entity_exists_in_world)
{
    return std::shared_ptr<EntitySharedBuilder>(new EntitySharedBuilder(this, std::move(name), std::move(entity_exists_in_world)));
}

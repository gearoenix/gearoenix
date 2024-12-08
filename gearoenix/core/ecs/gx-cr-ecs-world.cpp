#include "gx-cr-ecs-world.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../gx-cr-string.hpp"
#include <boost/mp11/algorithm.hpp>
#include <imgui.h>

namespace {
std::mutex world_lock;
auto world = std::make_unique<gearoenix::core::ecs::World>();

gearoenix::core::ecs::Archetype::id_t create_archetype_id(const gearoenix::core::ecs::EntityBuilder::components_t& cs)
{
    gearoenix::core::ecs::Archetype::id_t id;
    for (const auto& c : cs) {
        id.add(c.first);
        for (const auto& info = gearoenix::core::ecs::ComponentType::get_info(c.first); const auto pt : info.get_all_parents()) {
            id.add(pt);
        }
    }
    return id;
}
}

void gearoenix::core::ecs::World::read_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& stream,
    job::EndCaller<>&& end_callback)
{
    const auto count = stream->read<std::uint64_t>();
    for (auto i = decltype(count) { 0 }; i < count; ++i) {
        std::shared_ptr<platform::stream::Stream> ms = std::make_shared<platform::stream::Memory>();
        stream->read(*ms);
        const auto id = ms->read<entity_id_t>();
        job::send_job_to_pool([this, id, ms = std::move(ms), e = end_callback]() mutable {
            EntityBuilder::construct(
                id, std::move(ms),
                job::EndCallerShared<EntityBuilder>([this, end = std::move(e)](std::shared_ptr<EntityBuilder>&& eb) mutable {
                    eb->end_caller = std::move(end);
                    delayed_create_entity(std::move(*eb));
                }));
        });
    }
}

void gearoenix::core::ecs::World::write_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& stream,
    job::EndCaller<>&& end_callback) const
{
    auto entity_streams = std::make_shared<std::pair<std::mutex, std::vector<std::shared_ptr<platform::stream::Stream>>>>();
    job::EndCaller end([this, s = std::move(stream), e = std::move(end_callback), es = entity_streams]() mutable {
        const auto count = static_cast<std::uint64_t>(entities.size());
        GX_ASSERT(sizeof(count) == s->write(count));
        for (const auto& entity_stream : es->second) {
            platform::stream::Stream& ms = *entity_stream;
            s->write(ms);
        }
    });
    for (const auto& entity : entities) {
        job::send_job_to_pool([this, id = entity.first, end = end, es = entity_streams]() mutable {
            const auto& e = entities.find(id)->second;
            std::shared_ptr<platform::stream::Stream> s = std::make_shared<platform::stream::Memory>();
            GX_ASSERT(sizeof(id) == s->write<decltype(id)>(id));
            e.write(std::shared_ptr(s), job::EndCaller(end));
            std::lock_guard _lg(es->first);
            es->second.push_back(std::move(s));
        });
    }
}

gearoenix::core::ecs::World::World()
{
    const std::lock_guard _lg(world_lock);
    GX_ASSERT(nullptr == world); // only one world should exist!
}

gearoenix::core::ecs::World* gearoenix::core::ecs::World::get()
{
    return world.get();
}

void gearoenix::core::ecs::World::destroy()
{
    world = nullptr;
}

void gearoenix::core::ecs::World::create_entity(EntityBuilder&& b)
{
    auto* const a = get_archetype(b.all_types);
    auto* const cs = a->allocate_entity(b.id, b.all_types_to_components);
    entities.emplace(b.id, Entity(a, cs, std::string(b.name)));
    if (name_to_entity_id.contains(b.name)) {
        GX_LOG_F("Entity with name `" << b.name << "' is already in the world.");
    }
    name_to_entity_id.emplace(std::move(b.name), b.id);
}

void gearoenix::core::ecs::World::delayed_create_entity(EntityBuilder&& b)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action {
        .callback = job::EndCaller([] { }),
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
    const auto new_archetype = get_archetype(create_archetype_id(c));
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
    const entity_id_t id, const component_index_t* const ts, const std::uint64_t sz)
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
    for (auto ti = decltype(sz) { 0 }; ti < sz; ++ti) {
        const auto t = ts[ti];
        cs.erase(t);
        const auto& t_info = ComponentType::get_info(t);
        for (const auto pt : t_info.get_all_parents()) {
            cs.erase(pt);
        }
    }
    auto* const new_archetype = get_archetype(create_archetype_id(cs));
    entity->archetype = new_archetype;
    entity->components = new_archetype->allocate_entity(id, cs);
}

void gearoenix::core::ecs::World::delayed_remove_components_list(
    const entity_id_t ei,
    std::vector<component_index_t>&& cs,
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

gearoenix::core::ecs::Archetype* gearoenix::core::ecs::World::get_archetype(const Archetype::id_t& id)
{
    auto search = archetypes.find(id);
    if (archetypes.end() == search) {
        bool is_ok = false;
        std::tie(search, is_ok) = archetypes.emplace(id, std::unique_ptr<Archetype>(new Archetype(id)));
        if (!is_ok) {
            GX_LOG_F("Insertion in archetype map was not successful");
        }
    }
    return search->second.get();
}

gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const entity_id_t id)
{
    const auto search = entities.find(id);
    if (entities.end() == search) {
        return nullptr;
    }
    return &search->second;
}

const gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const entity_id_t id) const
{
    const auto search = entities.find(id);
    if (entities.end() == search) {
        return nullptr;
    }
    return &search->second;
}

gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const std::string& name)
{
    const auto search = name_to_entity_id.find(name);
    if (name_to_entity_id.end() == search) {
        return nullptr;
    }
    return get_entity(search->second);
}

const gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const std::string& name) const
{
    const auto search = name_to_entity_id.find(name);
    if (name_to_entity_id.end() == search) {
        return nullptr;
    }
    return get_entity(search->second);
}

void gearoenix::core::ecs::World::update()
{
    decltype(delayed_actions) actions;
    {
        const std::lock_guard _lg(delayed_actions_lock);
        std::swap(actions, delayed_actions);
    }

    constexpr std::uint32_t create_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::CreateEntity>::value;
    constexpr std::uint32_t delete_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::DeleteEntity>::value;
    constexpr std::uint32_t add_components_index = boost::mp11::mp_find<decltype(Action::variant), Action::AddComponents>::value;
    constexpr std::uint32_t delete_components_index = boost::mp11::mp_find<decltype(Action::variant), Action::DeleteComponents>::value;

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

void gearoenix::core::ecs::World::show_debug_gui() const
{
    if (ImGui::TreeNode(String::ptr_name(this).c_str())) {
        ImGui::Text("Number of Archetypes: %zu", archetypes.size());
        ImGui::Text("Number of Entities: %zu", entities.size());
        ImGui::Text("Number of Entity Names: %zu", name_to_entity_id.size());
        if (ImGui::TreeNode("Archetypes")) {
            for (const auto& a : archetypes) {
                if (ImGui::TreeNode(a.second->name.c_str())) {
                    for (const auto* const entity_ptr : a.second->entities) {
                        const auto entity_id = *reinterpret_cast<const entity_id_t*>(entity_ptr);
                        const auto search = entities.find(entity_id);
                        GX_ASSERT_D(search != entities.end());
                        search->second.show_debug_gui();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void gearoenix::core::ecs::World::read(const std::shared_ptr<platform::stream::Stream>& stream,
    job::EndCaller<>&& end_callback)
{
    job::send_job_to_pool([this, s = stream, e = std::move(end_callback)]() mutable {
        read_in_io_context(std::move(s), std::move(e));
    });
}

void gearoenix::core::ecs::World::write(
    const std::shared_ptr<platform::stream::Stream>& stream, job::EndCaller<>&& end_callback) const
{
    job::send_job_to_pool([this, s = stream, e = std::move(end_callback)]() mutable {
        write_in_io_context(std::move(s), std::move(e));
    });
}

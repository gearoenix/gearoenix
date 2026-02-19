#include "gx-cr-ecs-world.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../gx-cr-object-streamer.hpp"
#include "../gx-cr-string.hpp"
#include "gx-cr-ecs-entity.hpp"

#include <ImGui/imgui.h>
#include <boost/mp11/algorithm.hpp>

gearoenix::core::ecs::World::World()
    : Singleton(this)
{
    Object::register_type<Entity>();
}

gearoenix::core::ecs::World::~World()
{
    GX_ASSERT_D(resolvers.empty());
    GX_ASSERT_D(delayed_actions.empty());
}

void gearoenix::core::ecs::World::add_entity(EntityPtr&& e)
{
    get_archetype(e)->add_entity(e.get());
    const std::lock_guard _lg(entities_names_map_lock);
    const auto ptr = cast_shared<Entity>(e->get_object_self().lock());
    GX_ASSERT_D(ptr);
    entities_names_map[e->get_object_name()] = ptr;
}

void gearoenix::core::ecs::World::delayed_add_entity(EntityPtr&& e)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action { .variant = Action::Add { .entity = std::move(e) } });
}

void gearoenix::core::ecs::World::delete_entity(std::shared_ptr<Entity>&& e) { get_archetype(e.get())->delete_entity(e.get()); }

void gearoenix::core::ecs::World::delayed_delete_entity(std::shared_ptr<Entity>&& e)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action { .variant = Action::Delete { .entity = std::move(e) } });
}

void gearoenix::core::ecs::World::pullout_entity(job::EndCaller<EntityPtr>&& e)
{
    auto* const ptr = e.get_return().get();
    get_archetype(ptr)->delete_entity(ptr);
}

void gearoenix::core::ecs::World::delayed_pullout_entity(EntityPtr&& e, job::EndCaller<EntityPtr>&& end)
{
    end.set_return(std::move(e));
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action {
        .variant = Action::PullOut {
            .end_caller = std::move(end),
        },
    });
}

gearoenix::core::ecs::Archetype* gearoenix::core::ecs::World::get_archetype(const EntityPtr& e) { return get_archetype(e.get()); }

gearoenix::core::ecs::Archetype* gearoenix::core::ecs::World::get_archetype(const Entity* const e)
{
    const auto archetype_id = Archetype::create_id(e);
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        bool is_ok = false;
        std::tie(search, is_ok) = archetypes.emplace(archetype_id, std::unique_ptr<Archetype>(new Archetype(archetype_id)));
        GX_ASSERT_D(is_ok); // Insertion in the archetype map was not successful!
    }
    return search->second.get();
}

std::optional<gearoenix::core::ecs::EntityPtr> gearoenix::core::ecs::World::get_entity(const std::string& name)
{
    const std::lock_guard _lg(entities_names_map_lock);
    const auto search = entities_names_map.find(name);
    if (search == entities_names_map.end()) {
        return std::nullopt;
    }
    auto ptr = search->second.lock();
    if (nullptr == ptr) {
        return std::nullopt;
    }
    return std::make_optional(EntityPtr(std::move(ptr)));
}

void gearoenix::core::ecs::World::update()
{
    decltype(delayed_actions) actions;
    {
        const std::lock_guard _lg(delayed_actions_lock);
        std::swap(actions, delayed_actions);
    }

    constexpr auto add_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::Add>::value;
    constexpr auto delete_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::Delete>::value;
    constexpr auto pullout_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::PullOut>::value;
    constexpr auto read_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::Read>::value;
    constexpr auto write_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::Write>::value;

    for (auto& [variant] : actions) {
        switch (variant.index()) {
        case add_entity_index:
            add_entity(std::move(std::get<add_entity_index>(std::move(variant)).entity));
            break;
        case delete_entity_index:
            delete_entity(std::move(std::get<delete_entity_index>(std::move(variant)).entity));
            break;
        case pullout_entity_index: {
            auto&& [e] = std::move(std::get<pullout_entity_index>(std::move(variant)));
            pullout_entity(std::move(e));
            break;
        }
        case read_entity_index: {
            auto&& [s, end] = std::move(std::get<read_entity_index>(std::move(variant)));
            read(std::move(s), std::move(end));
            break;
        }
        case write_entity_index: {
            auto&& [s, end] = std::move(std::get<write_entity_index>(std::move(variant)));
            write(std::move(s), std::move(end));
            break;
        }
        default:
            GX_UNEXPECTED;
        }
    }

    decltype(resolvers) res;
    decltype(resolvers) rem_res;
    {
        const std::lock_guard _lg(resolvers_lock);
        std::swap(res, resolvers);
    }
    rem_res.reserve(res.size());
    for (auto& r : res) {
        if (r()) {
            rem_res.push_back(std::move(r));
        }
    }
    {
        const std::lock_guard _lg(resolvers_lock);
        resolvers.reserve(rem_res.size());
        for (auto& r : rem_res) {
            resolvers.push_back(std::move(r));
        }
    }
}

void gearoenix::core::ecs::World::show_debug_gui() const
{
    if (ImGui::TreeNode(String::ptr_name(this).c_str())) {
        ImGui::Text("Number of Archetypes: %zu", archetypes.size());
        if (ImGui::TreeNode("Archetypes")) {
            for (const auto& a : archetypes) {
                if (ImGui::TreeNode(a.second->name.c_str())) {
                    for (auto* const e : a.second->entities) {
                        e->show_debug_gui();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void gearoenix::core::ecs::World::resolve(resolver_t&& r)
{
    const std::lock_guard _lg(resolvers_lock);
    resolvers.push_back(std::move(r));
}

void gearoenix::core::ecs::World::clear()
{
    auto tries = 0;
    constexpr auto max_tries = 1000;
    while ((!resolvers.empty() || !delayed_actions.empty()) && ++tries < max_tries) {
        update();
    }
    GX_ASSERT_D(tries < max_tries);
    archetypes.clear();
}

void gearoenix::core::ecs::World::write(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end)
{
    if (std::scoped_lock _(delayed_actions_lock, resolvers_lock); !delayed_actions.empty() || !resolvers.empty()) {
        delayed_actions.push_back(Action { .variant = Action::Write { .stream = std::move(stream), .end_caller = std::move(end) } });
        return;
    }

    const std::shared_ptr<platform::stream::Stream> objects_stream = std::make_shared<platform::stream::Memory>();
    const auto object_streamer = ObjectStreamer::construct_writer(std::shared_ptr(objects_stream), job::EndCaller([stream, objects_stream, end = std::move(end)] -> void {
        stream->write(*objects_stream);
        (void)end;
    }));

    const auto archs_streams = std::make_shared<std::vector<std::shared_ptr<platform::stream::Stream>>>();
    const job::EndCaller archs_end([stream, object_streamer, archs_streams] -> void {
        for (const auto& arch_stream : *archs_streams) {
            stream->write(*arch_stream);
        }
        (void)object_streamer;
    });

    stream->write_fail_debug(static_cast<std::uint32_t>(archetypes.size()));
    for (const auto& [id, arch] : archetypes) {
        id.write(*stream);
        std::shared_ptr<platform::stream::Stream> arch_stream = std::make_shared<platform::stream::Memory>();
        archs_streams->push_back(arch_stream);
        arch->write(std::move(arch_stream), std::shared_ptr(object_streamer), job::EndCaller(archs_end));
    }
}

void gearoenix::core::ecs::World::read(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<std::vector<EntityPtr>>&& end)
{
    const auto archetypes_count = stream->read<std::uint32_t>();
    std::vector<component_index_set_t> archs_ids;
    std::vector<std::shared_ptr<platform::stream::Stream>> archs_streams;
    for (auto archetype_index = decltype(archetypes_count) { 0 }; archetype_index < archetypes_count; ++archetype_index) {
        archs_ids.emplace_back(*stream);
    }
    for (auto archetype_index = decltype(archetypes_count) { 0 }; archetype_index < archetypes_count; ++archetype_index) {
        std::shared_ptr<platform::stream::Stream> arch_stream = std::make_shared<platform::stream::Memory>();
        stream->read(*arch_stream);
        archs_streams.emplace_back(std::move(arch_stream));
    }
    std::shared_ptr<platform::stream::Stream> objects_stream = std::make_shared<platform::stream::Memory>();
    stream->read(*objects_stream);
    const auto object_streamer = ObjectStreamer::construct_reader(std::move(objects_stream));
    for (auto& arch_stream : archs_streams) {
        Archetype::read(std::move(arch_stream), std::shared_ptr(object_streamer), job::EndCaller(end));
    }
}
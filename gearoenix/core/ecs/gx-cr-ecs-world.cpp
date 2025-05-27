#include "gx-cr-ecs-world.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
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

void gearoenix::core::ecs::World::add_entity(Entity* const e)
{
    get_archetype(e)->add_entity(e);
}

void gearoenix::core::ecs::World::delayed_add_entity(Entity* const e)
{
    const std::lock_guard _lg(delayed_actions_lock);
    delayed_actions.push_back(Action { .variant = Action::Add { .entity = std::move(e) } });
}

void gearoenix::core::ecs::World::delete_entity(std::shared_ptr<Entity>&& e)
{
    get_archetype(e.get())->delete_entity(e.get());
}

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

gearoenix::core::ecs::Archetype* gearoenix::core::ecs::World::get_archetype(const EntityPtr& e)
{
    return get_archetype(e.get());
}

gearoenix::core::ecs::Archetype* gearoenix::core::ecs::World::get_archetype(const Entity* const e)
{
    const auto archetype_id = Archetype::create_id(e);
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        bool is_ok = false;
        std::tie(search, is_ok) = archetypes.emplace(archetype_id, std::unique_ptr<Archetype>(new Archetype(archetype_id)));
        GX_ASSERT_D("Insertion in archetype map was not successful");
    }
    return search->second.get();
}

void gearoenix::core::ecs::World::update()
{
    decltype(delayed_actions) actions;
    {
        const std::lock_guard _lg(delayed_actions_lock);
        std::swap(actions, delayed_actions);
    }

    constexpr auto create_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::Add>::value;
    constexpr auto delete_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::Delete>::value;
    constexpr auto pullout_entity_index = boost::mp11::mp_find<decltype(Action::variant), Action::PullOut>::value;

    for (auto& [variant] : actions) {
        switch (variant.index()) {
        case create_entity_index:
            add_entity(std::move(std::get<create_entity_index>(std::move(variant)).entity));
            break;
        case delete_entity_index:
            delete_entity(std::move(std::get<delete_entity_index>(std::move(variant)).entity));
            break;
        case pullout_entity_index: {
            auto&& [e] = std::move(std::get<pullout_entity_index>(std::move(variant)));
            pullout_entity(std::move(e));
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
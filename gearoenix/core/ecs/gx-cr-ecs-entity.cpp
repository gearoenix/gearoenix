#include "gx-cr-ecs-entity.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-table.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include "../gx-cr-object-streamer.hpp"
#include "../gx-cr-static-flat-set.hpp"
#include "gx-cr-ecs-world.hpp"

void gearoenix::core::ecs::Entity::write(
    std::shared_ptr<platform::stream::Stream>&& s,
    std::shared_ptr<ObjectStreamer>&& sc,
    job::EndCaller<>&&)
{
    static_flat_set<std::shared_ptr<Component>, max_components> components;
    for (const auto& ci : all_types_to_components) {
        components.emplace(ci.second);
    }

    s->write_fail_debug(object_name);

    s->write_fail_debug(static_cast<std::uint32_t>(components.size()));
    for (const auto& c : components) {
        s->write_fail_debug(c->get_object_id());
        sc->write(std::shared_ptr(c));
    }

    s->write_fail_debug(static_cast<std::uint32_t>(children.size()));
    for (const auto& c : children) {
        s->write_fail_debug(c.first);
        sc->write(c.second->get_object_self().lock());
    }
}

void gearoenix::core::ecs::Entity::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        Object::show_debug_gui();

        if (!all_types_to_components.empty() && ImGui::TreeNode("Components")) {
            static_flat_set<Component*, max_components> components;
            for (const auto& ci : all_types_to_components) {
                components.emplace(ci.second.get());
            }
            for (auto* const c : components) {
                c->show_debug_gui();
            }
            ImGui::TreePop();
        }

        if (!children.empty() && ImGui::TreeNode("Children")) {
            for (auto& c : children) {
                c.second->show_debug_gui();
            }
            ImGui::TreePop();
        }

        if (parent && ImGui::TreeNode("Parent")) {
            parent->show_debug_gui();
            ImGui::TreePop();
        }
    });
}

void gearoenix::core::ecs::Entity::add_component(std::shared_ptr<Component>&& component)
{
    // We should not be inside the world when we do this function
    GX_ASSERT_D(nullptr == archetype || all_types_to_components.empty());
    component->set_entity(this);
    const auto& all_parents = component->get_all_parent_types();
    const std::lock_guard _l(components_lock);
    for (const auto pt : all_parents) {
        GX_ASSERT_D(!all_types_to_components.contains(pt));
        all_types_to_components.emplace(pt, component);
    }
    const auto fti = component->get_object_final_type_index();
    GX_ASSERT_D(!all_types_to_components.contains(fti));
    all_types_to_components.emplace(fti, std::move(component));
}

void gearoenix::core::ecs::Entity::remove_component(const object_type_index_t ti)
{
    // We should not be inside the world when we do this function
    GX_ASSERT_D(nullptr == archetype);
    const auto& info = get_type_info(ti);
    const std::lock_guard _l(components_lock);
    all_types_to_components.erase(ti);
    for (const auto pt : info.get_all_parents()) {
        all_types_to_components.erase(pt);
    }
}

bool gearoenix::core::ecs::Entity::contains_in_parents(const Entity* const e) const
{
    for (const auto* p = parent; nullptr != p; p = p->parent) {
        if (e == p) {
            return true;
        }
    }
    return false;
}

gearoenix::core::ecs::EntityPtr gearoenix::core::ecs::Entity::construct(std::string&& name, Entity* const parent)
{
    EntityPtr entity(Object::construct<Entity>(std::move(name)));
    entity->set_parent(parent);
    return entity;
}

gearoenix::core::ecs::Entity::Entity(std::string&& name)
    : Object(gearoenix_core_ecs_entity_type_index, std::move(name))
{
}

gearoenix::core::ecs::Entity::~Entity()
{
    // Entity should have been deleted before here.
    // If you get this assert you, it means you're deleting the entity in a wrong way.
    GX_ASSERT_D(archetype == nullptr || !archetype->contains(this));
}

void gearoenix::core::ecs::Entity::set_parent(Entity* const p)
{
    if (parent == p) {
        return;
    }
    if (parent) {
        parent->children.erase(object_id);
    }
    parent = p;
    p->children.emplace(object_id, EntityPtr(std::static_pointer_cast<Entity>(object_self.lock())));
}

void gearoenix::core::ecs::Entity::add_child(EntityPtr&& child)
{
    if (child->parent == this) {
        return;
    }
    if (child->parent) {
        child->parent->children.erase(object_id);
    }
    child->parent = this;
    children.emplace(child->object_id, std::move(child));
}

void gearoenix::core::ecs::Entity::add_to_world()
{
    World::get().delayed_add_entity(this);
    for (auto& c : children) {
        c.second->add_to_world();
    }
}

const std::shared_ptr<gearoenix::core::ecs::Component>& gearoenix::core::ecs::Entity::get_component(const object_type_index_t component_type) const
{
    if (const auto search = all_types_to_components.find(component_type); search != all_types_to_components.end()) {
        return search->second;
    }
    static const std::shared_ptr<Component> null_comp = nullptr;
    return null_comp;
}

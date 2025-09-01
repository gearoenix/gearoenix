#pragma once
#include "gx-cr-ecs-archetype.hpp"
#include "gx-cr-ecs-entity.hpp"

namespace gearoenix::core::ecs {
template <typename Condition>
void find(const Entity& e, Entity::children_t& result)
{
    if (result.contains(e.get_object_id())) {
        return;
    }
    if (const auto* const archetype = e.get_archetype(); (archetype && archetype->satisfy<Condition>()) || e.satisfy<Condition>()) {
        result[e.get_object_id()] = EntityPtr(cast_shared<Entity>(e.get_object_self().lock()));
    }
    for (const auto& child : e.get_children() | std::views::values) {
        find<Condition>(*child.get(), result);
    }
}

template <typename Condition>
[[nodiscard]] Entity::children_t find(const Entity& e)
{
    Entity::children_t result;
    find<Condition>(e, result);
    return result;
}

template <typename Condition>
[[nodiscard]] Entity::children_t find(const EntityPtr& e)
{
    Entity::children_t result;
    find<Condition>(*e.get(), result);
    return result;
}
}
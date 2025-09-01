#include "gx-cr-ecs-archetype.hpp"
#include "gx-cr-ecs-comp-type.hpp"

gearoenix::core::ecs::Archetype::id_t gearoenix::core::ecs::Archetype::create_id(const Entity* entity)
{
    id_t id;
    for (const auto& ts : entity->get_all_types_to_components()) {
        id.add(ts.first);
    }
    return id;
}

gearoenix::core::ecs::Archetype::components_indices_t gearoenix::core::ecs::Archetype::create_components_indices(const id_t& id)
{
    components_indices_t cis;
    for (const auto type_index : id) {
        const auto ti = static_cast<object_type_index_t>(type_index);
        cis[ti] = 0;
        for (const auto& pts = Object::get_type_info(ti).get_all_parents(); const auto pt : pts) {
            cis[static_cast<object_type_index_t>(pt)] = 0;
        }
    }
    std::uint32_t index = 0;
    for (auto& ci : cis) {
        ci.second = index;
        ++index;
    }
    return cis;
}

std::string gearoenix::core::ecs::Archetype::create_name(const id_t& id)
{
    std::string s = "[ ";
    for (const auto type_index : id) {
        const auto& info = Object::get_type_info(static_cast<object_type_index_t>(type_index));
        if (!info.get_is_final()) {
            continue;
        }
        s += info.get_name();
        s += ", ";
    }
    s += "]";
    s.shrink_to_fit();
    return s;
}

gearoenix::core::ecs::Archetype::Archetype(const id_t& id)
    : id(id)
    , components_indices(create_components_indices(id))
    , name(create_name(id))
{
}

void gearoenix::core::ecs::Archetype::add_entity(Entity* const e)
{
    GX_ASSERT_D(id == create_id(e));

    if (e->archetype == this) {
        GX_ASSERT_D(entities.contains(e));
        return;
    }

    GX_ASSERT_D(e->archetype == nullptr); // entity must not be in another archetype
    e->archetype = this;

    const std::lock_guard _lg(entities_lock);
    entities.emplace(e);
}

void gearoenix::core::ecs::Archetype::delete_entity(Entity* const e)
{
    GX_ASSERT_D(id == create_id(e));
    GX_ASSERT_D(e->archetype == nullptr || e->archetype == this);

    e->archetype = nullptr;
    const std::lock_guard _lg(entities_lock);
    entities.erase(e);
}

bool gearoenix::core::ecs::Archetype::contains(Entity* const e) const
{
    return entities.contains(e);
}

gearoenix::core::ecs::Archetype::~Archetype()
{
    GX_ASSERT_D(entities.empty());
}

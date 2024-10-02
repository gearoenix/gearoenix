#include "gx-cr-ecs-archetype.hpp"
#include <cstring>

std::size_t gearoenix::core::ecs::Archetype::get_components_size(const EntityBuilder::components_t& cs)
{
    std::size_t s = 0;
    for (const auto& c : cs) {
        s += sizeof(c.second);
    }
    return s;
}

gearoenix::core::ecs::Archetype::components_indices_t gearoenix::core::ecs::Archetype::get_components_indices(
    const EntityBuilder::components_t& cs)
{
    components_indices_t cis;
    std::size_t index = 0;
    for (const auto& c : cs) {
        const auto& ts = c.second->get_hierarchy_types();
        for (const auto& t : ts) {
            if (const auto search = cis.find(t.first); cis.end() != search) {
                GX_LOG_F("There is a duplicate type in the entity components map.\n"
                         "No two component types can have overlapping component types in their hierarchy map.\n"
                         "Clashing component: "
                    << c.second->get_name());
            }
            cis.emplace(t.first, index);
        }
        ++index;
    }
    return cis;
}

std::string gearoenix::core::ecs::Archetype::create_name(const EntityBuilder::components_t& cs)
{
    std::string s = "[ ";
    for (const auto& c : cs) {
        const auto search = Component::get_type_index_to_name().find(c.first);
        GX_ASSERT_D(search != Component::get_type_index_to_name().end());
        s += search->second;
        s += " , ";
    }
    s += "]";
    s.shrink_to_fit();
    return s;
}

gearoenix::core::ecs::Archetype::Archetype(const EntityBuilder::components_t& cs)
    : components_indices(get_components_indices(cs))
    , entity_size(header_size + get_components_size(cs))
    , name(create_name(cs))
    , alc(entity_size, 2048)
{
}

std::shared_ptr<gearoenix::core::ecs::Component>* gearoenix::core::ecs::Archetype::allocate_entity(const entity_id_t id)
{
    unsigned char* const entity = alc.alloc();
    entities.emplace(entity);
    std::memset(entity, 0, entity_size); // we need it because of the std::shared_ptr moves
    *reinterpret_cast<entity_id_t*>(entity) = id;
    return reinterpret_cast<std::shared_ptr<Component>*>(&entity[header_size]);
}

std::shared_ptr<gearoenix::core::ecs::Component>* gearoenix::core::ecs::Archetype::allocate_entity(
    const entity_id_t ei,
    const EntityBuilder::components_t& cs)
{
    auto* const ptr = allocate_entity(ei);
    for (const auto& c : cs) {
        auto search = components_indices.find(c.first);
        GX_ASSERT_D(components_indices.end() != search);
        ptr[search->second] = c.second;
    }
    return ptr;
}

void gearoenix::core::ecs::Archetype::delete_entity(std::shared_ptr<Component>* const cs)
{
    for (const auto& ci : components_indices) {
        cs[ci.second] = nullptr;
    }
    auto* const ptr = reinterpret_cast<unsigned char*>(cs) - sizeof(entity_id_t);
    entities.erase(ptr);
    alc.free(ptr);
}

void gearoenix::core::ecs::Archetype::move_out_entity(
    std::shared_ptr<Component>* const cs,
    EntityBuilder::components_t& components)
{
    for (auto& ci : components_indices) {
        auto& c = cs[ci.second];
        if (nullptr == c) {
            continue;
        }
        const auto t = c->get_final_type_index();
        if (const auto search = components.find(t); components.end() != search) {
            GX_LOG_F("Component already exists in the component map, name: " << c->get_name());
        }
        components.emplace(t, std::move(c));
        c = nullptr;
    }
    delete_entity(cs);
}

gearoenix::core::ecs::Archetype::~Archetype()
{
    for (auto* const entity : entities) {
        auto* const ptr = reinterpret_cast<std::shared_ptr<Component>*>(&entity[header_size]);
        for (const auto& ci : components_indices) {
            ptr[ci.second] = nullptr;
        }
#ifdef GX_DEBUG_MODE
        alc.free(entity);
#endif
    }
}

#include "gx-cr-ecs-archetype.hpp"
#include <cstring>

std::uint32_t gearoenix::core::ecs::Archetype::create_components_size(const id_t& id)
{
    std::uint32_t s = 0;
    for (const auto ti : id) {
        if (Component::get_type_info(ti).get_is_final()) {
            s += static_cast<std::uint32_t>(sizeof(std::shared_ptr<Component>));
        }
    }
    return s;
}

gearoenix::core::ecs::Archetype::components_indices_t gearoenix::core::ecs::Archetype::create_components_indices(const id_t& id)
{
    components_indices_t cis;
    std::uint32_t index = 0;
    for (const auto ti : id) {
        const auto& info = Component::get_type_info(ti);
        if (!info.get_is_final()) {
            continue;
        }
        cis.emplace(ti, index);
        for (const auto& pts = info.get_all_parents(); const auto pt : pts) {
            cis.emplace(pt, index);
        }
        ++index;
    }
    return cis;
}

std::string gearoenix::core::ecs::Archetype::create_name(const id_t& id)
{
    std::string s = "[ ";
    for (const auto ti : id) {
        const auto& info = Component::get_type_info(ti);
        if (!info.get_is_final()) {
            continue;
        }
        s += info.get_name();
        s += " , ";
    }
    s += "]";
    s.shrink_to_fit();
    return s;
}

gearoenix::core::ecs::Archetype::Archetype(const id_t& id)
    : id(id)
    , components_indices(create_components_indices(id))
    , entity_size(header_size + create_components_size(id))
    , name(create_name(id))
    , alc(entity_size, 2048)
{
}

std::shared_ptr<gearoenix::core::ecs::Component>* gearoenix::core::ecs::Archetype::allocate_entity(const entity_id_t eid)
{
    unsigned char* const entity = alc.alloc();
    entities.emplace(entity);
    std::memset(entity, 0, entity_size); // we need it because of the std::shared_ptr moves
    *reinterpret_cast<entity_id_t*>(entity) = eid;
    return reinterpret_cast<std::shared_ptr<Component>*>(&entity[header_size]);
}

std::shared_ptr<gearoenix::core::ecs::Component>* gearoenix::core::ecs::Archetype::allocate_entity(
    const entity_id_t eid,
    const EntityBuilder::components_t& cs)
{
    auto* const ptr = allocate_entity(eid);
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

#include "gx-cr-ecs-archetype.hpp"
#include <cstring>

std::size_t gearoenix::core::ecs::Archetype::get_components_size(const EntityBuilder::components_t& cs) noexcept
{
    std::size_t s = 0;
    for (const auto& c : cs)
        s += Component::get_type_info(c.first).get_size();
    return s;
}

gearoenix::core::ecs::Archetype::components_indices_t gearoenix::core::ecs::Archetype::get_components_indices(
    const EntityBuilder::components_t& cs) noexcept
{
    const auto css = cs.size();
    components_indices_t cis;
    cis.reserve(css);
    std::size_t index = 0;
    for (const auto& c : cs) {
        cis[c.first] = index;
        index += Component::get_type_info(c.first).get_size();
    }
    return cis;
}

gearoenix::core::ecs::Archetype::Archetype(const EntityBuilder::components_t& cs) noexcept
    : components_indices(get_components_indices(cs))
    , entity_size(header_size + get_components_size(cs))
    , alc(entity_size, 2048)
{
}

unsigned char* gearoenix::core::ecs::Archetype::allocate_entity(const entity_id_t id) noexcept
{
    unsigned char* const entity = alc.alloc();
    entities.emplace(entity);
    *reinterpret_cast<entity_id_t*>(entity) = id;
    return entity + sizeof(entity_id_t);
}

unsigned char* gearoenix::core::ecs::Archetype::allocate_entity(
    const entity_id_t ei,
    const EntityBuilder::components_t& cs) noexcept
{
    auto* const ptr = allocate_entity(ei);
    for (const auto& c : cs) {
        auto search = components_indices.find(c.first);
        GX_ASSERT_D(components_indices.end() != search);
        Component::get_type_info(c.first).get_move_constructor()(&ptr[search->second], c.second.get());
    }
    return ptr;
}

void gearoenix::core::ecs::Archetype::remove_entity(unsigned char* const cs) noexcept
{
    for (const auto& ci : components_indices)
        reinterpret_cast<Component*>(cs + ci.second)->~Component();
    unsigned char* const ptr = cs - sizeof(entity_id_t);
    entities.erase(ptr);
    alc.free(ptr);
}

gearoenix::core::ecs::Archetype::~Archetype() noexcept
{
    for (auto* const entity : entities) {
        auto* const ptr = entity + sizeof(entity_id_t);
        for (const auto& ci : components_indices) {
            reinterpret_cast<Component*>(&ptr[ci.second])->~Component();
        }
#ifdef GX_DEBUG_MODE
        alc.free(entity);
#endif
    }
}

void gearoenix::core::ecs::Archetype::move_out_entity(
    unsigned char* const cs,
    EntityBuilder::components_t& components) noexcept
{
    for (const auto& ci : components_indices) {
        const auto& ti = Component::get_type_info(ci.first);
        std::unique_ptr<Component> dst(reinterpret_cast<Component*>(std::malloc(ti.get_size())));
        auto* const src = reinterpret_cast<Component*>(&cs[ci.second]);
        ti.get_move_constructor()(dst.get(), src);
        components.emplace(ci.first, std::move(dst));
    }
    remove_entity(cs);
}
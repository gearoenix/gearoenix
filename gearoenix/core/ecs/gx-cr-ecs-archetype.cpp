#include "gx-cr-ecs-archetype.hpp"
#include <cstring>

std::size_t gearoenix::core::ecs::Archetype::get_components_size(const EntityBuilder::components_t& cs) noexcept
{
    std::size_t s = 0;
    for (const auto& c : cs)
        s += c.second.size();
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
        index += c.second.size();
    }
    return cis;
}

gearoenix::core::ecs::Archetype::Archetype(const EntityBuilder::components_t& cs) noexcept
    : components_indices(get_components_indices(cs))
    , entity_size(header_size + get_components_size(cs))
{
}

std::uint8_t* gearoenix::core::ecs::Archetype::allocate_size(const std::size_t sz) noexcept
{
    const auto s = data.size();
    for (std::size_t i = 0; i < sz; ++i)
        data.push_back(0);
    return &data[s];
}

void gearoenix::core::ecs::Archetype::allocate_entity(const Entity::id_t id) noexcept
{
    allocate<Entity::id_t>() = id;
}

std::size_t gearoenix::core::ecs::Archetype::allocate_entity(
    const Entity::id_t ei,
    const EntityBuilder::components_t& cs) noexcept
{
    allocate_entity(ei);
    const std::size_t result = data.size();
    for (const auto& c : cs)
        data.insert(data.end(), c.second.begin(), c.second.end());
    return result;
}

std::optional<std::pair<gearoenix::core::ecs::Entity::id_t, std::size_t>> gearoenix::core::ecs::Archetype::remove_entity(
    const std::size_t index) noexcept
{
    for (const auto& ci : components_indices)
        reinterpret_cast<Component*>(&data[index + ci.second])->~Component();
    return move_from_back(index);
}

gearoenix::core::ecs::Archetype::Archetype(Archetype&& o) noexcept
    : components_indices(o.components_indices)
    , entity_size(o.entity_size)
    , data(std::move(o.data))
{
}

gearoenix::core::ecs::Archetype::~Archetype() noexcept
{
    for (std::size_t index = header_size; index < data.size(); index += entity_size)
        for (const auto& ci : components_indices)
            reinterpret_cast<Component*>(&data[index + ci.second])->~Component();
}

void gearoenix::core::ecs::Archetype::move_out_entity(
    const std::size_t index,
    EntityBuilder::components_t& components) noexcept
{
    if (components_indices.empty())
        return;
    for (auto iter_first = components_indices.begin(), iter_second = iter_first + 1; iter_second != components_indices.end(); ++iter_first, ++iter_second) {
        const auto ci = iter_first->second;
        const auto csz = iter_second->second - ci;
        std::vector<std::uint8_t> cd(csz);
        std::memcpy(cd.data(), &data[index + ci], csz);
        components.emplace(iter_first->first, std::move(cd));
    }
    auto back = components_indices.rbegin();
    const auto ci = back->second;
    const auto csz = (entity_size - header_size) - ci;
    std::vector<std::uint8_t> cd(csz);
    std::memcpy(cd.data(), &data[index + ci], csz);
    components.emplace(back->first, std::move(cd));
}

std::optional<std::pair<gearoenix::core::ecs::Entity::id_t, std::size_t>> gearoenix::core::ecs::Archetype::move_from_back(
    const std::size_t index) noexcept
{
    const auto start_index = index - header_size;
    if (start_index + entity_size < data.size()) {
        const auto end_index = data.size() - entity_size;
        std::memcpy(&data[start_index], &data[end_index], entity_size);
        data.resize(end_index);
        return std::make_pair(*reinterpret_cast<Entity::id_t*>(&data[start_index]), start_index + header_size);
    }
#ifdef GX_DEBUG_MODE
    if (start_index + entity_size != data.size())
        GX_UNEXPECTED;
#endif
    data.resize(start_index);
    return std::nullopt;
}
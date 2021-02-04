#include "gx-cr-ecs-archetype.hpp"

std::size_t gearoenix::core::ecs::Archetype::get_components_size(const Entity::Builder::components_t& cs) noexcept
{
    std::size_t s = 0;
    for (const auto& c : cs)
        s += c.second.size();
    return s;
}

gearoenix::core::ecs::Archetype::components_indices_t gearoenix::core::ecs::Archetype::get_components_indices(
    const Entity::Builder::components_t& cs) noexcept
{
    const auto css = cs.size();
    components_indices_t cis;
    cis.reserve(css);
    for (std::size_t i = 0, index = 0; i < css; ++i) {
        const auto& c = cs[i];
        const auto s = c.second.size();
        cis.emplace_back(c.first, index);
        index += s;
    }
    return cis;
}

gearoenix::core::ecs::Archetype::Archetype(const Entity::Builder::components_t& cs) noexcept
    : components_indices(get_components_indices(cs))
    , entity_size(header_size + get_components_size(cs))
{
}

gearoenix::core::ecs::Archetype::Archetype(
    const std::size_t components_size,
    components_indices_t&& components_indices) noexcept
    : components_indices(std::move(components_indices))
    , entity_size(header_size + components_size)
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
    const Entity::Builder::components_t& cs) noexcept
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
    Entity::Builder::components_t& components) noexcept
{
    for (std::size_t i = 0, j = 1; j < components_indices.size(); i = j, ++j) {
        const auto ci = components_indices[i].second;
        const auto csz = components_indices[j].second - ci;
        std::vector<std::uint8_t> cd(csz);
        std::memcpy(cd.data(), &data[index + ci], csz);
        components.emplace_back(components_indices[i].first, std::move(cd));
    }
    if (!components_indices.empty()) {
        auto& back = components_indices.back();
        const auto ci = back.second;
        const auto csz = (entity_size - header_size) - ci;
        std::vector<std::uint8_t> cd(csz);
        std::memcpy(cd.data(), &data[index + ci], csz);
        components.emplace_back(back.first, std::move(cd));
    }
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
        GX_UNEXPECTED
#endif
    data.resize(start_index);
    return std::nullopt;
}
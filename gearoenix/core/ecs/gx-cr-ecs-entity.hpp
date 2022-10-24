#ifndef GEAROENIX_CORE_ECS_ENTITY_HPP
#define GEAROENIX_CORE_ECS_ENTITY_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <atomic>
#include <boost/container/flat_map.hpp>
#include <optional>
#include <string>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct EntityBuilder;
struct Entity final {
    friend struct World;
    friend struct EntityBuilder;

    typedef entity_id_t id_t;

private:
    Entity(std::size_t archetype, std::size_t index_in_archetype, std::optional<std::string> name) noexcept;
    static std::atomic<id_t> last_id;
    std::size_t archetype = static_cast<std::size_t>(-1);
    std::size_t index_in_archetype = static_cast<std::size_t>(-1);
    std::optional<std::string> name;

public:
    Entity(Entity&&) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) noexcept = default;
    Entity& operator=(const Entity&) = delete;
};

struct EntityBuilder final {
    friend struct World;
    typedef boost::container::flat_map<std::type_index, std::vector<std::uint8_t>> components_t;

    GX_GET_CVAL_PRV(Entity::id_t, id);
    GX_GETSET_CREF_PRV(std::optional<std::string>, name);

private:
    components_t components;

    explicit EntityBuilder(Entity::id_t) noexcept;

public:
    EntityBuilder() noexcept;
    EntityBuilder(EntityBuilder&&) noexcept;
    EntityBuilder(const EntityBuilder&) = delete;
    EntityBuilder& operator=(EntityBuilder&&) = delete;
    EntityBuilder& operator=(const EntityBuilder&) = delete;

    template <typename ComponentType>
    void add_component(ComponentType&& component) noexcept
    {
        Component::types_check<ComponentType>();
        auto c = std::make_pair(
            Component::create_type_index<ComponentType>(),
            std::vector<std::uint8_t>(sizeof(ComponentType)));
        const auto search = components.find(c.first);
        if (search != components.end())
            GX_LOG_F("Component '" << typeid(ComponentType).name() << "' already exists in entity '" << id);
        new (c.second.data()) ComponentType(std::forward<ComponentType>(component));
        components.emplace(std::move(c));
    }

    template <typename... ComponentType>
    void add_components(ComponentType&&... cs) noexcept
    {
        ((add_component(std::move(cs))), ...);
    }

    [[nodiscard]] const void* get_component(std::type_index component_type) const noexcept;
    [[nodiscard]] void* get_component(std::type_index component_type) noexcept;

    template <typename ComponentType>
    const ComponentType* get_component() const noexcept
    {
        return reinterpret_cast<const ComponentType*>(get_component(std::type_index(typeid(ComponentType))));
    }

    template <typename ComponentType>
    ComponentType* get_component() noexcept
    {
        return reinterpret_cast<ComponentType*>(get_component(std::type_index(typeid(ComponentType))));
    }
};

struct EntitySharedBuilder final {
    friend struct World;
    GX_GET_REF_PRV(EntityBuilder, builder);

private:
    World* const world;

    explicit EntitySharedBuilder(World* world) noexcept;

public:
    ~EntitySharedBuilder() noexcept;
};
}

#endif
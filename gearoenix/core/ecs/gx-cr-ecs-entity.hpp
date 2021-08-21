#ifndef GEAROENIX_CORE_ECS_ENTITY_HPP
#define GEAROENIX_CORE_ECS_ENTITY_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <atomic>
#include <optional>
#include <string>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct EntityBuilder;
struct Entity final {
    friend struct World;
    friend struct EntityBuilder;

    typedef std::uint32_t id_t;

private:
    Entity(std::size_t archetype, std::size_t index_in_archetype) noexcept;
    static std::atomic<id_t> last_id;
    std::size_t archetype = static_cast<std::size_t>(-1);
    std::size_t index_in_archetype = static_cast<std::size_t>(-1);

public:
    Entity(Entity&&) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) = delete;
    Entity& operator=(const Entity&) = delete;
};

struct EntityBuilder final {
    friend struct World;
    typedef std::pair<std::type_index, std::vector<std::uint8_t>> component_t;
    typedef std::vector<component_t> components_t;

private:
    GX_GET_CVAL_PRV(Entity::id_t, id)
    GX_GETSET_CREF_PRV(std::optional<std::string>, name)
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
        const auto search = std::lower_bound(
            components.begin(), components.end(), c.first,
            [](const component_t& l, const std::type_index r) noexcept {
                return l.first < r;
            });
        if (search != components.end() && search->first == c.first)
            GX_LOG_F("Component '" << typeid(ComponentType).name() << "' already exists in entity '" << id)
        new (c.second.data()) ComponentType(std::forward<ComponentType>(component));
        components.insert(search, std::move(c));
    }

    template <typename... ComponentType>
    void add_components(ComponentType&&... cs) noexcept
    {
        ((add_component(std::move(cs))), ...);
    }

    [[nodiscard]] const void* get_component(std::type_index component_type) const noexcept;
    [[nodiscard]] void* get_component(std::type_index component_type) noexcept;

    template <typename ComponentType>
    std::optional<std::reference_wrapper<const ComponentType>> get_component() const noexcept
    {
        const void* const ptr = get_component(std::type_index(typeid(ComponentType)));
        if (nullptr == ptr)
            return std::nullopt;
        const ComponentType& comp = *reinterpret_cast<const ComponentType*>(ptr);
        return comp;
    }

    template <typename ComponentType>
    std::optional<std::reference_wrapper<ComponentType>> get_component() noexcept
    {
        void* const ptr = get_component(std::type_index(typeid(ComponentType)));
        if (nullptr == ptr)
            return std::nullopt;
        ComponentType& comp = *reinterpret_cast<ComponentType*>(ptr);
        return comp;
    }

    static void sort(components_t&) noexcept;
};

struct EntitySharedBuilder final {
    friend struct World;
    GX_GET_REF_PRV(EntityBuilder, builder)

private:
    World* const world;

    explicit EntitySharedBuilder(World* world) noexcept;

public:
    ~EntitySharedBuilder() noexcept;
};
}

#endif
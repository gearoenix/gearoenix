#ifndef GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#define GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../allocator/gx-cr-alc-same-size-block.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../gx-cr-range.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../sync/gx-cr-sync-parallel-for.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-condition.hpp"
#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <functional>
#include <map>
#include <optional>
#include <type_traits>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct Entity;
struct Archetype final {
    friend struct World;
    friend struct Entity;

private:
    template <typename T>
    struct ConditionCheck final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices) noexcept
        {
            return components_indices.contains(Component::create_type_index<T>());
        }
    };

    template <typename Condition>
    struct ConditionCheck<Not<Condition>> final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices) noexcept
        {
            return !ConditionCheck<Condition>::match(components_indices);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<And<Conditions...>> final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices) noexcept
        {
            return (ConditionCheck<Conditions>::match(components_indices) && ...);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<Or<Conditions...>> final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices) noexcept
        {
            return (ConditionCheck<Conditions>::match(components_indices) || ...);
        }
    };

    typedef boost::container::flat_set<std::type_index> id_t;
    typedef boost::container::flat_map<std::type_index, std::size_t> components_indices_t;

    constexpr static std::size_t header_size = sizeof(entity_id_t);

    const components_indices_t components_indices;
    const std::size_t entity_size;
    allocator::SameSizeBlock alc;
    boost::container::flat_set<std::uint8_t*> entities;

    template <typename... ComponentsTypes>
    [[nodiscard]] static id_t create_id() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        id_t id {
            Component::create_type_index<ComponentsTypes>()...,
        };
        GX_ASSERT(id.size() == sizeof...(ComponentsTypes)); // TODO this can be done in compile time
        return id;
    }

    template <typename Condition>
    [[nodiscard]] bool satisfy() noexcept
    {
        return ConditionCheck<Condition>::match(components_indices);
    }

    template <typename... ComponentsTypes>
    [[nodiscard]] constexpr static std::size_t get_components_size() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        std::size_t size = 0;
        ((size += sizeof(ComponentsTypes)), ...);
        return size;
    }

    [[nodiscard]] static std::size_t get_components_size(const EntityBuilder::components_t&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] static components_indices_t get_components_indices() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        std::size_t index = 0;
        components_indices_t indices {
            { Component::create_type_index<ComponentsTypes>(), sizeof(ComponentsTypes) }...,
        };
        for (auto& i : indices) {
            auto s = i.second;
            i.second = index;
            index += s;
        }
        return indices;
    }

    [[nodiscard]] static components_indices_t get_components_indices(const EntityBuilder::components_t&) noexcept;

    explicit Archetype(const EntityBuilder::components_t&) noexcept;

    Archetype(const std::size_t components_size, components_indices_t&& components_indices) noexcept
        : components_indices(std::move(components_indices))
        , entity_size(header_size + components_size)
        , alc(entity_size, 2048)
    {
    }

    template <typename... ComponentsTypes>
    [[nodiscard]] static Archetype* create() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        return new Archetype(get_components_size<ComponentsTypes...>(), get_components_indices<ComponentsTypes...>());
    }

    [[nodiscard]] unsigned char* allocate_entity(entity_id_t) noexcept;

    [[nodiscard]] unsigned char* allocate_entity(entity_id_t, const EntityBuilder::components_t&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] unsigned char* allocate(const entity_id_t id, ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        unsigned char* const ptr = allocate_entity(id);
        if constexpr (sizeof...(ComponentsTypes) > 0) {
            ((new (ptr + get_component_index<ComponentsTypes>()) ComponentsTypes(std::move(components))), ...);
        }
        return ptr;
    }

    void remove_entity(unsigned char* cs) noexcept;

    /// If it fails to find the the component it will return -1
    template <typename T>
    [[nodiscard]] std::size_t get_component_index() const noexcept
    {
        Component::type_check<T>();
        const auto search = components_indices.find(Component::create_type_index<T>());
        if (components_indices.end() == search)
            return static_cast<std::size_t>(-1);
        return search->second;
    }

    /// Returns null if it fails to find the component
    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component(unsigned char* const components) noexcept
    {
        Component::type_check<ComponentType>();
        const auto search = components_indices.find(Component::create_type_index<ComponentType>());
        if (components_indices.end() == search)
            return nullptr;
        return reinterpret_cast<ComponentType*>(components + search->second);
    }

    template <typename ComponentType>
    [[nodiscard]] const ComponentType* get_component(const unsigned char* const components) const noexcept
    {
        Component::type_check<ComponentType>();
        const auto search = components_indices.find(Component::create_type_index<ComponentType>());
        if (components_indices.end() == search)
            return nullptr;
        return reinterpret_cast<const ComponentType*>(components + search->second);
    }

    void move_out_entity(unsigned char* cs, EntityBuilder::components_t& components) noexcept;

    template <typename ComponentsTypesTuple, std::size_t... I, typename F>
    void parallel_system(std::index_sequence<I...> const&, F&& fun) noexcept
    {
        const std::size_t indices[] = {
            (sizeof(entity_id_t) + get_component_index<std::tuple_element_t<I, ComponentsTypesTuple>>())...,
        };
        sync::ParallelFor::exec(entities.begin(), entities.end(), [&](std::uint8_t* const ptr, const auto kernel_index) noexcept {
            const auto id = *reinterpret_cast<const entity_id_t*>(ptr);
            fun(id, reinterpret_cast<std::tuple_element_t<I, ComponentsTypesTuple>*>(indices[I] >= sizeof(entity_id_t) ? &ptr[indices[I]] : nullptr)..., kernel_index);
        });
    }

    template <typename Condition, typename F>
    void parallel_system_conditioned(F&& fun) noexcept
    {
        parallel_system<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>(), fun);
    }

    template <typename ComponentsTypesTuple, std::size_t... I, typename F>
    void synchronised_system(std::index_sequence<I...> const&, F&& fun) noexcept
    {
        const std::size_t indices[] = {
            (sizeof(entity_id_t) + get_component_index<std::tuple_element_t<I, ComponentsTypesTuple>>())...,
        };
        for (auto ptr : entities) {
            const auto id = *reinterpret_cast<const entity_id_t*>(ptr);
            fun(id, reinterpret_cast<std::tuple_element_t<I, ComponentsTypesTuple>*>(indices[I] >= sizeof(entity_id_t) ? &ptr[indices[I]] : nullptr)...);
        }
    }

    template <typename Condition, typename F>
    void synchronised_system_conditioned(F&& fun) noexcept
    {
        synchronised_system<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>(), fun);
    }

public:
    Archetype(const Archetype&) = delete;
    ~Archetype() noexcept;
};
}

#endif

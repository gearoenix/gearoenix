#ifndef GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#define GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../gx-cr-range.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../sync/gx-cr-sync-parallel-for.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-not.hpp"
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
struct Archetype final {
    friend struct World;

private:
    typedef boost::container::flat_set<std::type_index> id_t;
    typedef boost::container::flat_map<std::type_index, std::size_t> components_indices_t;

    constexpr static std::size_t header_size = sizeof(Entity::id_t);

    const components_indices_t components_indices;
    const std::size_t entity_size;
    std::vector<std::uint8_t> data;

    template <typename... ComponentsTypes>
    [[nodiscard]] static id_t create_id() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        id_t id {
            Component::create_type_index<ComponentsTypes>()...,
        };
        GX_ASSERT(id.size() == sizeof...(ComponentsTypes))
        return id;
    }

    template <typename... ComponentsTypes>
    [[nodiscard]] bool satisfy() noexcept
    {
        Component::query_types_check<ComponentsTypes...>();
        bool result = true;
        (([&]<typename T>(T*) constexpr noexcept {
            result = result && (is_not<T> != components_indices.contains(Component::create_type_index<T>()));
        }(reinterpret_cast<ComponentsTypes*>(0))),
            ...);
        return result;
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
    {
    }

    template <typename... ComponentsTypes>
    [[nodiscard]] static constexpr Archetype create() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        return Archetype(get_components_size<ComponentsTypes...>(), get_components_indices<ComponentsTypes...>());
    }

    [[nodiscard]] std::uint8_t* allocate_size(std::size_t) noexcept;

    template <typename T>
    T& allocate() noexcept
    {
        return *reinterpret_cast<T*>(allocate_size(sizeof(T)));
    }

    void allocate_entity(Entity::id_t) noexcept;

    [[nodiscard]] std::size_t allocate_entity(Entity::id_t, const EntityBuilder::components_t&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] std::size_t allocate(const Entity::id_t id, ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        allocate_entity(id);
        const auto result = data.size();
        if constexpr (sizeof...(ComponentsTypes) > 0) {
            auto* const ptr = allocate_size(get_components_size<ComponentsTypes...>());
            ((new (&ptr[get_component_index<ComponentsTypes>()]) ComponentsTypes(std::move(components))), ...);
        }
        return result;
    }

    std::optional<std::pair<Entity::id_t, std::size_t>> remove_entity(std::size_t index) noexcept;

    /// If it fails to find the the component it will return -1
    template <typename T>
    [[nodiscard]] std::size_t get_component_index() noexcept
    {
        Component::query_type_check<T>();
        const auto search = components_indices.find(Component::create_type_index<T>());
        if (components_indices.end() == search)
            return static_cast<std::size_t>(-1);
        return search->second;
    }

    /// Returns null if it fails to find the component
    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component(const std::size_t index) noexcept
    {
        Component::type_check<ComponentType>();
        const auto ci = get_component_index<ComponentType>();
        if (ci == static_cast<decltype(ci)>(-1))
            return nullptr;
        return reinterpret_cast<ComponentType*>(&data[index + ci]);
    }

    template <typename ComponentType>
    [[nodiscard]] const ComponentType* get_component(const std::size_t index) const noexcept
    {
        Component::type_check<ComponentType>();
        const auto ci = get_component_index<ComponentType>();
        if (ci == static_cast<decltype(ci)>(-1))
            return nullptr;
        return reinterpret_cast<const ComponentType*>(&data[index + ci]);
    }

    void move_out_entity(std::size_t index, EntityBuilder::components_t& components) noexcept;

    [[nodiscard]] std::optional<std::pair<Entity::id_t, std::size_t>> move_from_back(std::size_t index) noexcept;

    template <typename... C, std::size_t... I, std::size_t N, typename F>
    static inline void call_function(
        std::index_sequence<I...> const&,
        const Entity::id_t id,
        std::uint8_t* const ptr,
        const std::size_t (&indices)[N],
        F&& f,
        const unsigned int kernel_index) noexcept
    {
        f(id, *reinterpret_cast<C*>(&ptr[indices[I]])..., kernel_index);
    }

    template <typename... C, std::size_t... I, std::size_t N, typename F>
    static inline void call_function_sync(
        std::index_sequence<I...> const&,
        const Entity::id_t id,
        std::uint8_t* const ptr,
        const std::size_t (&indices)[N],
        F&& f) noexcept
    {
        f(id, *reinterpret_cast<C*>(&ptr[indices[I]])...);
    }

    template <typename... ComponentsTypes, typename F>
    void parallel_system(F&& fun) noexcept
    {
        const std::size_t indices[] = {
            (is_not<ComponentsTypes> ? 0 : (sizeof(Entity::id_t) + get_component_index<ComponentsTypes>()))...,
        };
        auto range = PtrRange(data.data(), data.size(), entity_size);
        sync::ParallelFor::exec(range.begin(), range.end(), [&](std::uint8_t* const ptr, const unsigned int kernel_index) noexcept {
            const auto id = *reinterpret_cast<const Entity::id_t*>(ptr);
            call_function<ComponentsTypes...>(
                std::make_index_sequence<sizeof...(ComponentsTypes)> {}, id, ptr, indices, fun, kernel_index);
        });
    }

    template <typename... ComponentsTypes, typename F>
    void synchronised_system(F&& fun) noexcept
    {
        const std::size_t indices[] = {
            (is_not<ComponentsTypes> ? 0 : (sizeof(Entity::id_t) + get_component_index<ComponentsTypes>()))...,
        };
        auto range = PtrRange(data.data(), data.size(), entity_size);
        for (auto iter = range.begin(); iter != range.end(); ++iter) {
            std::uint8_t* const ptr = *iter;
            const auto id = *reinterpret_cast<const Entity::id_t*>(ptr);
            call_function_sync<ComponentsTypes...>(
                std::make_index_sequence<sizeof...(ComponentsTypes)> {}, id, ptr, indices, fun);
        }
    }

public:
    Archetype(Archetype&&) noexcept;
    Archetype(const Archetype&) = delete;
    ~Archetype() noexcept;
};
}

#endif
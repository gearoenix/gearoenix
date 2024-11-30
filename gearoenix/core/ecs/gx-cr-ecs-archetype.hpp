#pragma once
#include "../allocator/gx-cr-alc-same-size-block.hpp"
#include "../gx-cr-range.hpp"
#include "../sync/gx-cr-sync-parallel-for.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-condition.hpp"
#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-types.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <type_traits>

namespace gearoenix::core::ecs {
struct World;
struct Entity;
struct Archetype final {
    friend struct World;
    friend struct Entity;

    typedef Component::TypeIndexSet id_t;
    typedef boost::container::flat_map<Component::TypeIndex, std::uint32_t> components_indices_t;

    template <typename T>
    struct ConditionCheck final {
        [[nodiscard]] constexpr static bool match(const Component::TypeIndexSet& id)
        {
            return id.contains(Component::create_type_index<T>());
        }
    };

    template <typename Condition>
    struct ConditionCheck<Not<Condition>> final {
        [[nodiscard]] constexpr static bool match(const Component::TypeIndexSet& id)
        {
            return !ConditionCheck<Condition>::match(id);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<All<Conditions...>> final {
        [[nodiscard]] constexpr static bool match(const Component::TypeIndexSet& id)
        {
            return (ConditionCheck<Conditions>::match(id) && ...);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<Any<Conditions...>> final {
        [[nodiscard]] constexpr static bool match(const Component::TypeIndexSet& id)
        {
            return (ConditionCheck<Conditions>::match(id) || ...);
        }
    };

    constexpr static std::uint32_t header_size = sizeof(entity_id_t);

    const id_t id;
    const components_indices_t components_indices;
    const std::uint32_t entity_size;
    const std::string name;

private:
    allocator::SameSizeBlock alc;
    boost::container::flat_set<std::uint8_t*> entities;

    [[nodiscard]] static std::uint32_t create_components_size(const id_t&);

    [[nodiscard]] static components_indices_t create_components_indices(const id_t&);

    [[nodiscard]] static std::string create_name(const id_t&);

    explicit Archetype(const id_t&);

    [[nodiscard]] std::shared_ptr<Component>* allocate_entity(entity_id_t);

    [[nodiscard]] std::shared_ptr<Component>* allocate_entity(entity_id_t, const EntityBuilder::components_t&);

    void delete_entity(std::shared_ptr<Component>* cs);

    /// If it fails to find the component it will return -1
    template <typename T>
    [[nodiscard]] std::uint32_t get_component_index() const
    {
        const auto search = components_indices.find(Component::create_type_index<T>());
        if (components_indices.end() == search) {
            return static_cast<std::uint32_t>(-1);
        }
        return search->second;
    }

    /// Returns null if it fails to find the component
    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component(const std::shared_ptr<Component>* const components) const
    {
        const auto search = components_indices.find(Component::create_type_index<ComponentType>());
        if (components_indices.end() == search) {
            return nullptr;
        }
        if constexpr (std::is_base_of_v<Component, ComponentType>) {
            return static_cast<ComponentType*>(components[search->second].get());
        } else {
            return dynamic_cast<ComponentType*>(components[search->second].get());
        }
    }

    template <typename ComponentType>
    [[nodiscard]] std::shared_ptr<ComponentType> get_component_shared_ptr(const std::shared_ptr<Component>* const components) const
    {
        const auto search = components_indices.find(Component::create_type_index<ComponentType>());
        if (components_indices.end() == search) {
            return nullptr;
        }
        if constexpr (std::is_base_of_v<Component, ComponentType>) {
            return std::static_pointer_cast<ComponentType>(components[search->second]);
        } else {
            return std::dynamic_pointer_cast<ComponentType>(components[search->second]);
        }
    }

    void move_out_entity(std::shared_ptr<Component>* cs, EntityBuilder::components_t& components);

    template <typename ComponentType, std::uint32_t I>
    static ComponentType* get_component_ptr(const std::shared_ptr<Component>* cs, const std::uint32_t* const is)
    {
        const auto index = is[I];
        if (static_cast<std::uint32_t>(-1) == index) {
            return nullptr;
        }
        Component* const rc = cs[index].get();
        if constexpr (std::is_final_v<ComponentType>) {
            return static_cast<ComponentType*>(rc);
        } else {
            return dynamic_cast<ComponentType*>(rc);
        }
    }

    template <typename ComponentsTypesTuple, std::uint32_t... I, typename F>
    void parallel_system(std::index_sequence<I...> const&, F&& fun)
    {
        const std::array<std::uint32_t, sizeof...(I)> indices = {
            (get_component_index<std::tuple_element_t<I, ComponentsTypesTuple>>())...,
        };
        sync::ParallelFor::exec(entities.begin(), entities.end(), [&](const std::uint8_t* const ptr, const auto kernel_index) {
            const auto id = *reinterpret_cast<const entity_id_t*>(ptr);
            const auto* const cs = reinterpret_cast<const std::shared_ptr<Component>*>(&ptr[header_size]);
            fun(id, get_component_ptr<std::tuple_element_t<I, ComponentsTypesTuple>, I>(cs, indices.data())..., kernel_index);
        });
    }

    template <typename Condition, typename F>
    void parallel_system_conditioned(F&& fun)
    {
        parallel_system<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>(), fun);
    }

    template <typename ComponentsTypesTuple, std::uintptr_t... I, typename F>
    void synchronised_system(std::index_sequence<I...> const&, F&& fun)
    {
        const std::array<std::uint32_t, sizeof...(I)> indices = {
            (get_component_index<std::tuple_element_t<I, ComponentsTypesTuple>>())...,
        };
        for (const auto* const ptr : entities) {
            const auto entity_id = *reinterpret_cast<const entity_id_t*>(ptr);
            const auto* const cs = reinterpret_cast<const std::shared_ptr<Component>*>(&ptr[header_size]);
            fun(entity_id, get_component_ptr<std::tuple_element_t<I, ComponentsTypesTuple>, I>(cs, indices.data())...);
        }
    }

    template <typename Condition, typename F>
    void synchronised_system_conditioned(F&& fun)
    {
        synchronised_system<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>(), fun);
    }

public:
    template <typename Condition>
    [[nodiscard]] constexpr bool satisfy()
    {
        return ConditionCheck<Condition>::match(id);
    }

    Archetype(const Archetype&) = delete;
    ~Archetype();
};
}
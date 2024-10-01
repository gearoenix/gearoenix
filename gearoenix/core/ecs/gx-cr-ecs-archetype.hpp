#ifndef GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#define GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../allocator/gx-cr-alc-same-size-block.hpp"
#include "../gx-cr-range.hpp"
#include "../sync/gx-cr-sync-parallel-for.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-condition.hpp"
#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-types.hpp"

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include <array>
#include <type_traits>

namespace gearoenix::core::ecs {
struct World;
struct Entity;
struct Archetype final {
    friend struct World;
    friend struct Entity;

private:
    template <typename T>
    struct ConditionCheck final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices)
        {
            return components_indices.contains(Component::create_type_index<T>());
        }
    };

    template <typename Condition>
    struct ConditionCheck<Not<Condition>> final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices)
        {
            return !ConditionCheck<Condition>::match(components_indices);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<All<Conditions...>> final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices)
        {
            return (ConditionCheck<Conditions>::match(components_indices) && ...);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<Any<Conditions...>> final {
        [[nodiscard]] static bool match(const boost::container::flat_map<std::type_index, std::size_t>& components_indices)
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

    template <typename Condition>
    [[nodiscard]] bool satisfy()
    {
        return ConditionCheck<Condition>::match(components_indices);
    }

    [[nodiscard]] static std::size_t get_components_size(const EntityBuilder::components_t&);

    [[nodiscard]] static components_indices_t get_components_indices(const EntityBuilder::components_t&);

    explicit Archetype(const EntityBuilder::components_t&);

    [[nodiscard]] std::shared_ptr<Component>* allocate_entity(entity_id_t);

    [[nodiscard]] std::shared_ptr<Component>* allocate_entity(entity_id_t, const EntityBuilder::components_t&);

    void delete_entity(std::shared_ptr<Component>* cs);

    /// If it fails to find the component it will return -1
    template <typename T>
    [[nodiscard]] std::size_t get_component_index() const
    {
        const auto search = components_indices.find(Component::create_type_index<T>());
        if (components_indices.end() == search) {
            return static_cast<std::size_t>(-1);
        }
        return search->second;
    }

    /// Returns null if it fails to find the component
    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component(const std::shared_ptr<Component>* const components) const
    {
        const auto search = components_indices.find(Component::create_type_index<ComponentType>());
        if (components_indices.end() == search)
            return nullptr;
        if constexpr (std::is_final_v<ComponentType>) {
            return static_cast<ComponentType*>(components[search->second].get());
        } else {
            return dynamic_cast<ComponentType*>(components[search->second].get());
        }
    }

    void move_out_entity(std::shared_ptr<Component>* cs, EntityBuilder::components_t& components);

    template <typename ComponentType, std::size_t I>
    inline static ComponentType* get_component_ptr(const std::shared_ptr<Component>* cs, const std::size_t* const is)
    {
        const std::size_t index = is[I];
        if (static_cast<std::size_t>(-1) == index)
            return nullptr;
        Component* const rc = cs[index].get();
        if constexpr (std::is_final_v<ComponentType>) {
            return static_cast<ComponentType*>(rc);
        } else {
            return dynamic_cast<ComponentType*>(rc);
        }
    }

    template <typename ComponentsTypesTuple, std::size_t... I, typename F>
    void parallel_system(std::index_sequence<I...> const&, F&& fun)
    {
        const std::array<std::size_t, sizeof...(I)> indices = {
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

    template <typename ComponentsTypesTuple, std::size_t... I, typename F>
    void synchronised_system(std::index_sequence<I...> const&, F&& fun)
    {
        const std::array<std::size_t, sizeof...(I)> indices = {
            (get_component_index<std::tuple_element_t<I, ComponentsTypesTuple>>())...,
        };
        for (const auto* const ptr : entities) {
            const auto id = *reinterpret_cast<const entity_id_t*>(ptr);
            const auto* const cs = reinterpret_cast<const std::shared_ptr<Component>*>(&ptr[header_size]);
            fun(id, get_component_ptr<std::tuple_element_t<I, ComponentsTypesTuple>, I>(cs, indices.data())...);
        }
    }

    template <typename Condition, typename F>
    void synchronised_system_conditioned(F&& fun)
    {
        synchronised_system<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>(), fun);
    }

public:
    Archetype(const Archetype&) = delete;
    ~Archetype();
};
}

#endif

#pragma once
#include "../sync/gx-cr-sync-parallel-for.hpp"
#include "gx-cr-ecs-condition.hpp"
#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-types.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

namespace gearoenix::core::ecs {
struct Archetype final {
    friend struct World;
    friend struct Entity;

    typedef component_index_set_t id_t;
    typedef boost::container::flat_map<object_type_index_t, std::uint32_t> components_indices_t;

    template <typename T>
    struct ConditionCheck final {
        [[nodiscard]] constexpr static bool match(const component_index_set_t& id)
        {
            return id.contains(T::object_type_index);
        }
    };

    template <typename Condition>
    struct ConditionCheck<Not<Condition>> final {
        [[nodiscard]] constexpr static bool match(const component_index_set_t& id)
        {
            return !ConditionCheck<Condition>::match(id);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<All<Conditions...>> final {
        [[nodiscard]] constexpr static bool match(const component_index_set_t& id)
        {
            return (ConditionCheck<Conditions>::match(id) && ...);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<Any<Conditions...>> final {
        [[nodiscard]] constexpr static bool match(const component_index_set_t& id)
        {
            return (ConditionCheck<Conditions>::match(id) || ...);
        }
    };

    const id_t id;
    const components_indices_t components_indices;
    const std::string name;

private:
    std::mutex entities_lock;
    boost::container::flat_set<Entity*> entities;

    [[nodiscard]] static id_t create_id(const Entity* entity);
    [[nodiscard]] static components_indices_t create_components_indices(const id_t&);
    [[nodiscard]] static std::string create_name(const id_t&);

    explicit Archetype(const id_t&);

    void add_entity(Entity*);
    void delete_entity(Entity*);
    [[nodiscard]] bool contains(Entity*) const;

    /// If it fails to find the component, it will return -1
    template <typename T>
    [[nodiscard]] std::uint32_t get_component_index() const
    {
        const auto search = components_indices.find(T::object_type_index);
        if (components_indices.end() == search) {
            return static_cast<std::uint32_t>(-1);
        }
        return search->second;
    }

    template <typename T, std::size_t I>
    [[nodiscard]] static T* get_component(const Entity* const e, const std::uint32_t* const indices)
    {
        const auto i = indices[I];
        return i != static_cast<std::uint32_t>(-1) ? static_cast<T*>((e->get_all_types_to_components().begin() + i)->second.get()) : nullptr;
    }

    template <typename ComponentsTypesTuple, std::uintptr_t... I, typename F>
    void parallel_system(std::index_sequence<I...> const&, const F& fun)
    {
        const std::array indices = {
            (get_component_index<std::tuple_element_t<I, ComponentsTypesTuple>>())...,
        };
        sync::parallel_for(entities, [&](auto* const e, const auto kernel_index) {
            fun(e, get_component<std::tuple_element_t<I, ComponentsTypesTuple>, I>(e, indices.data())..., kernel_index);
        });
    }

    template <typename Condition, typename F>
    void parallel_system_conditioned(const F& fun)
    {
        using types = typename ConditionTypesPack<Condition>::types;
        parallel_system<types>(std::make_index_sequence<std::tuple_size_v<types>>(), fun);
    }

    template <typename ComponentsTypesTuple, std::uintptr_t... I, typename F>
    void synchronised_system(std::index_sequence<I...> const&, const F& fun)
    {
        const std::array indices = {
            (get_component_index<std::tuple_element_t<I, ComponentsTypesTuple>>())...,
        };
        for (auto* const e : entities) {
            fun(e, get_component<std::tuple_element_t<I, ComponentsTypesTuple>, I>(e, indices.data())...);
        }
    }

    template <typename Condition, typename F>
    void synchronised_system_conditioned(const F& fun)
    {
        using types = typename ConditionTypesPack<Condition>::types;
        synchronised_system<types>(std::make_index_sequence<std::tuple_size_v<types>>(), fun);
    }

public:
    template <typename Condition>
    [[nodiscard]] constexpr bool satisfy() const
    {
        return ConditionCheck<Condition>::match(id);
    }

    Archetype(const Archetype&) = delete;
    Archetype(Archetype&&) = delete;
    ~Archetype();
};
}

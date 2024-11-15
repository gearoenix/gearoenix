#ifndef GEAROENIX_CORE_ECS_COMPONENT_HPP
#define GEAROENIX_CORE_ECS_COMPONENT_HPP
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-condition.hpp"
#include "gx-cr-ecs-types.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/core/demangle.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::core::ecs {
struct World;
struct Component {
    typedef boost::container::flat_map<std::type_index, std::string> HierarchyTypes;

    GX_GET_REFC_PRT(std::type_index, final_type_index);
    GX_GET_REFC_PRT(std::string, name);
    GX_GETSET_VAL_PRV(bool, enabled, true);
    GX_GET_CREF_PRV(std::weak_ptr<Component>, component_self);
    GX_GETSET_VAL_PRT(entity_id_t, entity_id, INVALID_ENTITY_ID);

    static boost::container::flat_map<std::type_index, std::string> type_index_to_name;
    static boost::container::flat_map<std::string, std::type_index> type_name_to_index;

    static void register_type(std::type_index t, std::string name);

public:
    Component(std::type_index final_type_index, std::string&& name, entity_id_t entity_id);
    [[nodiscard]] virtual const HierarchyTypes& get_hierarchy_types() const = 0;
    virtual void set_component_self(const std::shared_ptr<Component>&);

    virtual ~Component() = default;
    Component(Component&&) = delete;
    Component(const Component&) = delete;
    Component& operator=(Component&&) = delete;
    Component& operator=(const Component&) = delete;

    virtual void show_debug_gui(const render::engine::Engine&);

    template <typename T>
    constexpr static void type_check()
    {
        static_assert(!std::is_reference_v<T>, "Component type can not be a reference type.");
        static_assert(!std::is_move_assignable_v<T>, "Component type can not be move assignable.");
        static_assert(!std::is_move_constructible_v<T>, "Component type must be move constructable.");
        static_assert(!std::is_nothrow_move_constructible_v<T>, "Component type must be nothrow move constructable.");
        static_assert(!std::is_copy_assignable_v<T>, "Component type can not be copy assignable.");
        static_assert(!std::is_copy_constructible_v<T>, "Component type can not be copy constructable.");
        static_assert(std::is_final_v<T>, "Component type must be final and has no child");
        static_assert(std::is_base_of_v<Component, T>, "Component type must be inherited from gearoenix::core::ecs::Component");
        static_assert(!is_not_v<T>, "Component type can not be gearoenix::core::ecs::Not");
        static_assert(!is_all_v<T>, "Component type can not be gearoenix::core::ecs::And");
        static_assert(!is_any_v<T>, "Component type can not be gearoenix::core::ecs::Or");
    }

    template <typename... T>
    constexpr static void types_check()
    {
        ((type_check<T>()), ...);
    }

    template <typename Tuple, std::size_t... I>
    constexpr static void tuple_types_check(std::index_sequence<I...> const&)
    {
        ((type_check<std::tuple_element_t<I, Tuple>>()), ...);
    }

    template <typename Condition>
    constexpr static void condition_types_check()
    {
        tuple_types_check<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>());
    }

    template <typename T>
    static std::type_index create_type_index()
    {
        return std::type_index(typeid(not_t<T>));
    }

    template <typename T>
    static std::type_index create_this_type_index(const T* const)
    {
        using DT = std::remove_pointer_t<std::remove_reference_t<std::remove_const_t<T>>>;
        type_check<DT>();
        return std::type_index(typeid(not_t<DT>));
    }

    template <typename T>
    static std::string create_this_type_name(const T* const)
    {
        using DT = std::remove_pointer_t<std::remove_reference_t<std::remove_const_t<T>>>;
        type_check<DT>();
        return boost::core::demangle(typeid(not_t<DT>).name());
    }

    template <typename... Types, typename T>
    [[nodiscard]] static HierarchyTypes generate_hierarchy_types(const T* const ptr)
    {
        static_assert((std::is_base_of_v<Types, T> && ...));
        static_assert((!std::is_same_v<Types, T> && ...));
        static_assert((!std::is_same_v<Types, Component> && ...));
        register_type(create_this_type_index(ptr), create_this_type_name(ptr));
        (register_type(std::type_index(typeid(Types)), typeid(Types).name()), ...);
        return {
            { create_this_type_index(ptr), create_this_type_name(ptr) },
            { std::type_index(typeid(Types)), typeid(Types).name() }...,
        };
    }

    static const boost::container::flat_map<std::type_index, std::string>& get_type_index_to_name() { return type_index_to_name; }

    static const boost::container::flat_map<std::string, std::type_index>& get_type_name_to_index() { return type_name_to_index; }
};
}
#endif
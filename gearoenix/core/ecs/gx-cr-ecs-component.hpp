#ifndef GEAROENIX_CORE_ECS_COMPONENT_HPP
#define GEAROENIX_CORE_ECS_COMPONENT_HPP
#include "gx-cr-ecs-condition.hpp"
#include <cstdint>
#include <type_traits>
#include <typeindex>

namespace gearoenix::core::ecs {
struct Component {
    const std::type_index type_index;
    bool enabled = true;

    template <typename T>
    explicit Component(T*) noexcept
        : type_index(Component::create_type_index<T>())
    {
        type_check<T>();
    }

    virtual ~Component() noexcept = default;
    Component(Component&&) noexcept = default;
    Component(const Component&) = delete;
    Component& operator=(Component&&) = delete;
    Component& operator=(const Component&) = delete;

    virtual void display() noexcept { }

    template <typename T>
    constexpr static void type_check() noexcept
    {
        static_assert(!std::is_reference_v<T>, "Component type can not be a reference type.");
        static_assert(!std::is_move_assignable_v<T>, "Component type can not be move assignable.");
        static_assert(std::is_move_constructible_v<T>, "Component type must be move constructible.");
        static_assert(!std::is_copy_assignable_v<T>, "Component type can not be copy assignable.");
        static_assert(!std::is_copy_constructible_v<T>, "Component type can not be copy constructible.");
        static_assert(std::is_final_v<T>, "Component type must be final and has no child");
        static_assert(std::is_base_of_v<Component, T>, "Component type must be inherited from gearoenix::core::ecs::Component");
        static_assert(!is_not_v<T>, "Component type can not be gearoenix::core::ecs::Not");
        static_assert(!is_and_v<T>, "Component type can not be gearoenix::core::ecs::And");
        static_assert(!is_or_v<T>, "Component type can not be gearoenix::core::ecs::Or");
    }

    template <typename... T>
    constexpr static void types_check() noexcept
    {
        ((type_check<T>()), ...);
    }

    template <typename Tuple, std::size_t... I>
    constexpr static void tuple_types_check(std::index_sequence<I...> const&) noexcept
    {
        ((type_check<std::tuple_element_t<I, Tuple>>()), ...);
    }

    template <typename Condition>
    constexpr static void condition_types_check() noexcept
    {
        tuple_types_check<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>());
    }

    template <typename T>
    static std::type_index create_type_index() noexcept
    {
        type_check<T>();
        return std::type_index(typeid(not_t<T>));
    }
};
}
#endif
#ifndef GEAROENIX_CORE_ECS_COMPONENT_HPP
#define GEAROENIX_CORE_ECS_COMPONENT_HPP
#include "gx-cr-ecs-not.hpp"
#include <cstdint>
#include <type_traits>
#include <typeindex>

namespace gearoenix::core::ecs {
struct Component {
    const std::type_index type_index;
    bool enabled = true;

    template <typename T>
    explicit Component(T*) noexcept;
    virtual ~Component() noexcept = default;
    virtual void display() noexcept { }

    template <typename T>
    constexpr static void type_check() noexcept;

    template <typename... T>
    constexpr static void types_check() noexcept;

    template <typename T>
    constexpr static void query_type_check() noexcept;

    template <typename... T>
    constexpr static void query_types_check() noexcept;

    template <typename T>
    static std::type_index create_type_index() noexcept;
};
}

template <typename T>
gearoenix::core::ecs::Component::Component(T*) noexcept
    : type_index(Component::create_type_index<T>())
{
    type_check<T>();
}

template <typename T>
constexpr void gearoenix::core::ecs::Component::type_check() noexcept
{
    static_assert(!std::is_reference_v<T>, "Component type can not be a reference type.");
    static_assert(!std::is_move_assignable_v<T>, "Component type can not be move assignable.");
    static_assert(std::is_move_constructible_v<T>, "Component type must be move constructible.");
    static_assert(!std::is_copy_assignable_v<T>, "Component type can not be copy assignable.");
    static_assert(!std::is_copy_constructible_v<T>, "Component type can not be copy constructible.");
    static_assert(std::is_final_v<T>, "Component type must be final and has no child");
    static_assert(std::is_base_of_v<Component, T>, "Component type must be inherited from gearoenix::core::ecs::Component");
    static_assert(!is_not<T>, "Component type can not be gearoenix::core::ecs::Not");
}

template <typename... T>
constexpr void gearoenix::core::ecs::Component::types_check() noexcept
{
    ((type_check<T>()), ...);
}

template <typename T>
constexpr void gearoenix::core::ecs::Component::query_type_check() noexcept
{
    if constexpr (is_not<T>)
        type_check<not_t<T>>();
    else
        type_check<T>();
}

template <typename... T>
constexpr void gearoenix::core::ecs::Component::query_types_check() noexcept
{
    ((query_type_check<T>()), ...);
}

template <typename T>
std::type_index gearoenix::core::ecs::Component::create_type_index() noexcept
{
    query_type_check<T>();
    return std::type_index(typeid(not_t<T>));
}

#endif
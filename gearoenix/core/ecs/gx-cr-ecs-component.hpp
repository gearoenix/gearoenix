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
};
template <typename T>
inline constexpr bool is_component = !std::is_reference_v<T> && !std::is_move_assignable_v<T> && std::is_move_constructible_v<T> && !std::is_copy_assignable_v<T> && !std::is_copy_constructible_v<T> && std::is_final_v<T> && std::is_base_of_v<Component, T> && !is_not<T>;

template <typename T>
inline constexpr bool is_component_query = (is_not<T> && is_component<typename IsNot<T>::type>) || is_component<T>;
}

template <typename T>
gearoenix::core::ecs::Component::Component(T*) noexcept
    : type_index(std::type_index(typeid(T)))
{
    type_check<T>();
}

template <typename T>
constexpr void gearoenix::core::ecs::Component::type_check() noexcept
{
    static_assert(is_component<T>, "Type must be compatible with component definition.");
}

template <typename... T>
constexpr void gearoenix::core::ecs::Component::types_check() noexcept
{
    ((type_check<T>()), ...);
}

template <typename T>
constexpr void gearoenix::core::ecs::Component::query_type_check() noexcept
{
    static_assert(is_component_query<T>, "Type must be compatible with component query definition.");
}

template <typename... T>
constexpr void gearoenix::core::ecs::Component::query_types_check() noexcept
{
    ((query_type_check<T>()), ...);
}

#endif
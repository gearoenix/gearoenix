#ifndef GEAROENIX_CORE_ECS_COMPONENT_HPP
#define GEAROENIX_CORE_ECS_COMPONENT_HPP
#include "gx-cr-ecs-not.hpp"
#include <cstdint>
#include <type_traits>
#include <typeindex>

namespace gearoenix::core::ecs {
struct Component;

template <typename T>
concept ComponentConcept = !std::is_reference_v<T> && !std::is_move_assignable_v<T> && std::is_move_constructible_v<T> && !std::is_copy_assignable_v<T> && !std::is_copy_constructible_v<T> && std::is_final_v<T> && std::is_base_of_v<Component, T> && !IsNot<T>::value;

template <typename T>
concept ComponentQueryConcept = (IsNot<T>::value && ComponentConcept<typename IsNot<T>::type>) || ComponentConcept<T>;

struct Component {
    const std::type_index type_index;
    bool enabled = true;

    template <ComponentConcept T>
    explicit Component(T*) noexcept;

    virtual ~Component() noexcept = default;
    virtual void display() noexcept { }
};
}

template <gearoenix::core::ecs::ComponentConcept T>
gearoenix::core::ecs::Component::Component(T*) noexcept
    : type_index(std::type_index(typeid(T)))
{
}

#endif
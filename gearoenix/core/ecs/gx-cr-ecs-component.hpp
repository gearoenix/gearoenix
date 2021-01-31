#ifndef GEAROENIX_CORE_ECS_COMPONENT_HPP
#define GEAROENIX_CORE_ECS_COMPONENT_HPP
#include "gx-cr-ecs-not.hpp"
#include <cstdint>
#include <type_traits>

namespace gearoenix::core::ecs {
struct Component {
    bool enabled = true;

    virtual ~Component() noexcept = default;
    virtual void display() noexcept { }

    template <typename T>
    static void type_check() noexcept
    {
        static_assert(!std::is_reference_v<T>, "Component argument(s) can not be reference types.");
        static_assert(std::is_base_of_v<Component, T>, "Component argument(s) must be derived from Component struct.");
        static_assert(!IsNot<T>::value, "Component argument(s) can not be Not type.");
    }
};
}

#endif
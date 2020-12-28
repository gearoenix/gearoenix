#ifndef GEAROENIX_CORE_ECS_TYPES_HPP
#define GEAROENIX_CORE_ECS_TYPES_HPP

#include <cstdint>

namespace gearoenix::core::ecs {
typedef std::uint32_t entity_id_t;

template <typename T>
struct Not {
    typedef T type;
};

template <typename T>
struct IsNot {
    typedef T type;
    static constexpr bool value = false;
};

template <typename T>
struct IsNot<Not<T>> {
    typedef T type;
    static constexpr bool value = true;
};

template <typename... Args>
struct CountNot {
};

template <typename T, typename... Args>
struct CountNot<T, Args...> {
    static constexpr int value = (IsNot<T>::value ? 1 : 0) + CountNot<Args...>::value;
};

template <>
struct CountNot<> {
    static constexpr int value = 0;
};
}

#endif
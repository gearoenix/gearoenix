#ifndef GEAROENIX_CORE_ECS_NOT_HPP
#define GEAROENIX_CORE_ECS_NOT_HPP

namespace gearoenix::core::ecs {
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

template <typename T>
inline constexpr bool is_not = IsNot<T>::value;

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
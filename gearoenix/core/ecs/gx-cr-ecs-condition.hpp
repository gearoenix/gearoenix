#ifndef GEAROENIX_CORE_ECS_CONDITION_HPP
#define GEAROENIX_CORE_ECS_CONDITION_HPP
#include <tuple>
#include <type_traits>

namespace gearoenix::core::ecs {
template <typename T>
struct Not final {
    typedef T type;
};

template <typename T>
struct IsNot final {
    typedef T type;
    static constexpr bool value = false;
};

template <typename T>
struct IsNot<Not<T>> final {
    typedef T type;
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool is_not_v = IsNot<T>::value;

template <typename T>
using not_t = typename IsNot<T>::type;

template <typename... Args>
struct CountNot final {
};

template <typename T, typename... Args>
struct CountNot<T, Args...> final {
    static constexpr int value = (is_not_v<T> ? 1 : 0) + CountNot<Args...>::value;
};

template <>
struct CountNot<> final {
    static constexpr int value = 0;
};

template <typename... Args>
struct And final {
    static constexpr std::size_t count = sizeof...(Args);
    template <std::size_t N>
    using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

template <typename T>
struct IsAnd final {
    typedef T type;
    static constexpr bool value = false;
};

template <typename... Args>
struct IsAnd<And<Args...>> final {
    template <std::size_t N>
    using type = typename And<Args...>::template type<N>;
    static constexpr bool value = true;
    using tuple = std::tuple<Args...>;
    static constexpr std::size_t count = sizeof...(Args);
};

template <typename T>
inline constexpr bool is_and_v = IsAnd<T>::value;

template <typename T, std::size_t N>
using and_t = typename IsAnd<T>::template type<N>;

template <typename... Args>
struct Or final {
    static constexpr std::size_t count = sizeof...(Args);
    template <std::size_t N>
    using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

template <typename T>
struct IsOr final {
    typedef T type;
    static constexpr bool value = false;
};

template <typename... Args>
struct IsOr<Or<Args...>> final {
    template <std::size_t N>
    using type = typename Or<Args...>::template type<N>;
    static constexpr bool value = true;
    using tuple = std::tuple<Args...>;
    static constexpr std::size_t count = sizeof...(Args);
};

template <typename T>
inline constexpr bool is_or_v = IsOr<T>::value;

template <typename T, std::size_t N>
using or_t = typename IsOr<T>::template type<N>;

template <typename>
inline constexpr bool is_tuple_v = false;

template <typename... T>
inline constexpr bool is_tuple_v<std::tuple<T...>> = true;

template <typename Tuple1, typename Tuple2>
struct unique_merge final {
};

template <typename... Ts>
struct unique_merge<std::tuple<Ts...>, std::tuple<>> final {
    typedef std::tuple<Ts...> type;
};

template <typename... Ts, typename U, typename... Us>
struct unique_merge<std::tuple<Ts...>, std::tuple<U, Us...>> final {
    typedef std::conditional_t<(std::is_same_v<U, Ts> || ...), typename unique_merge<std::tuple<Ts...>, std::tuple<Us...>>::type, typename unique_merge<std::tuple<Ts..., U>, std::tuple<Us...>>::type> type;
};

template <typename T>
struct ConditionTypesPack final {

    template <typename Tuple>
    struct TuplesPack final {
        using types = Tuple;
    };

    template <typename FirstT, typename... Ts>
    struct TuplesPack<std::tuple<FirstT, Ts...>> final {
        using types = typename unique_merge<typename ConditionTypesPack<FirstT>::types, typename TuplesPack<std::tuple<Ts...>>::types>::type;
    };

    template <typename TT>
    struct Types final {
        using tuple = std::tuple<TT>;
    };

    template <typename TT>
    struct Types<Not<TT>> final {
        using tuple = typename ConditionTypesPack<TT>::types;
    };

    template <typename... Ts>
    struct Types<And<Ts...>> final {
        using tuple = typename TuplesPack<typename std::tuple<Ts...>>::types;
    };

    template <typename... Ts>
    struct Types<Or<Ts...>> final {
        using tuple = typename TuplesPack<typename std::tuple<Ts...>>::types;
    };

    using types = typename Types<T>::tuple;
};
static_assert(std::is_same_v<std::tuple<int, char, float>, ConditionTypesPack<And<int, char, float>>::types>, "Internal error!");
}

#endif
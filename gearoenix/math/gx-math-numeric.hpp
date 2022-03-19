#ifndef GEAROENIX_MATH_NUMERIC_HPP
#define GEAROENIX_MATH_NUMERIC_HPP

#include "../core/gx-cr-types.hpp"
#include <cmath>
#include <limits>

namespace gearoenix::math {
struct Numeric {
public:
    template <typename T>
    constexpr static void check_signable() noexcept
    {
        static_assert(
            std::numeric_limits<T>::is_signed,
            "This functionality only works when the type is signable.");
    }

    template <typename T1, typename T2>
    constexpr static void check_same_signibility() noexcept
    {
        static_assert(
            !std::numeric_limits<T1>::is_signed || std::numeric_limits<T2>::is_signed,
            "This functionality only works when both of the type have signedness compatibility.");
    }

    // It will raise given number to the nearest bigger or equal value that is in power of 2
    template <typename T>
    [[nodiscard]] constexpr static T raise_p2(const T v) noexcept
    {
        unsigned long long i = 1;
        while (v > static_cast<T>(i)) {
            i <<= 1u;
        }
        return static_cast<T>(i);
    }

    template <typename T>
    [[nodiscard]] constexpr static T raise_p2(const T v, const T maximum, const T minimum) noexcept
    {
        if (v > maximum)
            return maximum;
        if (v < minimum)
            return minimum;
        return raise_p2(v);
    }

    template <typename T>
    [[nodiscard]] constexpr static T safe_maximum(const T a, const T b) noexcept
    {
        if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(a))
                return b;
            if (std::isnan(b))
                return a;
        }
        return a > b ? a : b;
    }

    template <typename T>
    [[nodiscard]] constexpr static T safe_minimum(const T a, const T b) noexcept
    {
        if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(a))
                return b;
            if (std::isnan(b))
                return a;
        }
        return a > b ? b : a;
    }

    template <typename T>
    [[nodiscard]] constexpr static T clamp(const T v, const T mx, const T mn) noexcept
    {
        return maximum(minimum(v, mx), mn);
    }

    /// On failure it returns static_cast<T>(-1).
    template <typename T>
    [[nodiscard]] constexpr static T floor_log2(const T a) noexcept
    {
        static_assert(std::numeric_limits<T>::is_integer, "Only integer number can be used by this function.");
        for (T i = a, j = 0; i > 0; i >>= 1, ++j) {
            if (i == 1) {
                return j;
            }
        }
        return static_cast<T>(-1);
    }

    [[nodiscard]] static constexpr float radical_inverse_vdc(std::uint32_t bits) noexcept
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return float(bits) * 2.3283064365386963e-10f; // / 0x100000000 // TODO
    }

    template <typename T>
    [[nodiscard]] static constexpr T geometry_schlick_ggx(const T n_dot_v, const T roughness) noexcept
    {
        const auto k = (roughness * roughness) * static_cast<T>(0.5);
        return n_dot_v / (n_dot_v * (1.0f - k) + k);
    }

    template <typename T>
    [[nodiscard]] static constexpr T align(const T value, const T alignment) noexcept
    {
        const T r = value % alignment;
        return r != 0 ? value + (alignment - r) : value;
    }
};
}
#endif
#ifndef GEAROENIX_MATH_NUMERIC_HPP
#define GEAROENIX_MATH_NUMERIC_HPP

#include "../core/cr-types.hpp"

// Maybe in future needed
#define GX_SIGNEDNESS_COMPATIBILITY_CHECK(ReceivedType, FinalType)                                            \
    static_assert(!std::numeric_limits<ReceivedType>::is_signed || std::numeric_limits<FinalType>::is_signed, \
        "This functionality only works when the signedness compatibility is available.");

#define GX_SIGNEDNESS_CHECK(Type)                       \
    static_assert(std::numeric_limits<Type>::is_signed, \
        "This functionality only works when the signedness compatibility is available.");

namespace gearoenix::math {
class Numeric {
public:
    // It will raise given number to the nearest bigger or equal value that is in power of 2
    template <typename T>
    static T raise_p2(T v) noexcept;
    template <typename T>
    static T raise_p2(T v, T maximum, T minimum) noexcept;

    [[nodiscard]] static constexpr float radical_inverse_vdc(std::uint32_t bits) noexcept;
};
}

template <typename T>
T gearoenix::math::Numeric::raise_p2(const T v) noexcept
{
    unsigned long long i = 1;
    while (v > static_cast<T>(i)) {
        i <<= 1;
    }
    return static_cast<T>(i);
}

template <typename T>
T gearoenix::math::Numeric::raise_p2(const T v, const T maximum, const T minimum) noexcept
{
    if (v > maximum)
        return maximum;
    if (v < minimum)
        return minimum;
    return raise_p2(v);
}

constexpr float gearoenix::math::Numeric::radical_inverse_vdc(std::uint32_t bits) noexcept
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000 // TODO
}

#endif
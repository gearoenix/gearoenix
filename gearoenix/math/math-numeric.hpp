#ifndef GEAROENIX_MATH_NUMERIC_HPP
#define GEAROENIX_MATH_NUMERIC_HPP

namespace gearoenix::math {
class Numeric {
public:
    // It will raise given number to the nearest bigger or equal value that is in power of 2
    template <typename T>
    static T raise_p2(T v) noexcept;
    template <typename T>
    static T raise_p2(T v, T maximun, T minimum) noexcept;
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
T gearoenix::math::Numeric::raise_p2(const T v, const T maximun, const T minimum) noexcept
{
    if (v > maximun)
        return maximun;
    if (v < minimum)
        return minimum;
    return raise_p2(v);
}

#endif
#pragma once
#include <bit>
#include <cstdint>

namespace gearoenix::math {
/// IEEE-754 single-precision -> half-precision (binary16) bit pattern, packed as `std::uint16_t`.
/// Round-toward-zero on the mantissa (no rounding bit), and approximate handling of subnormals
/// (values below ~6e-5 flush to signed zero). Inf/NaN are preserved.
[[nodiscard]] constexpr std::uint16_t to_half(const float f) noexcept
{
    const auto u = std::bit_cast<std::uint32_t>(f);
    const std::uint32_t sign = (u >> 16u) & 0x8000u;
    const std::int32_t exponent = static_cast<std::int32_t>((u >> 23) & 0xffu) - 127 + 15;
    const std::uint32_t mantissa = u & 0x7fffffu;
    if (exponent >= 31) {
        return static_cast<std::uint16_t>(sign | 0x7c00u | (mantissa ? 0x200u : 0u));
    }
    if (exponent <= 0) {
        if (exponent < -10) {
            return static_cast<std::uint16_t>(sign);
        }
        const std::uint32_t m = mantissa | 0x800000u;
        const std::uint32_t shifted = m >> (14 - exponent);
        return static_cast<std::uint16_t>(sign | shifted);
    }
    return static_cast<std::uint16_t>(sign | (static_cast<std::uint32_t>(exponent) << 10) | (mantissa >> 13));
}

/// Half-precision (binary16) bit pattern -> IEEE-754 single-precision. Inverse of `to_half` (modulo
/// the mantissa rounding `to_half` drops). Handles subnormals, signed zeros, inf and NaN.
[[nodiscard]] constexpr float from_half(const std::uint16_t h) noexcept
{
    const std::uint32_t sign = static_cast<std::uint32_t>(h & 0x8000u) << 16;
    const std::uint32_t exp_bits = (h >> 10) & 0x1fu;
    const std::uint32_t mant_bits = h & 0x3ffu;
    if (exp_bits == 0) {
        if (mant_bits == 0) {
            return std::bit_cast<float>(sign);
        }
        // Subnormal: shift to find the leading 1 then re-emit as a normal float.
        std::uint32_t m = mant_bits;
        std::int32_t e = -14;
        while ((m & 0x400u) == 0) {
            m <<= 1;
            --e;
        }
        m &= 0x3ffu;
        return std::bit_cast<float>(sign | (static_cast<std::uint32_t>(e + 127) << 23) | (m << 13));
    }
    if (exp_bits == 31) {
        return std::bit_cast<float>(sign | 0x7f800000u | (mant_bits << 13));
    }
    return std::bit_cast<float>(sign | ((exp_bits + (127u - 15u)) << 23) | (mant_bits << 13));
}
}
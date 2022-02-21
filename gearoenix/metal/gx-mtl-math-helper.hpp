#ifndef GEAROENIX_METAL_MATH_HELPER_HPP
#define GEAROENIX_METAL_MATH_HELPER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import <simd/simd.h>
#import "../math/gx-math-vector-4d.hpp"

namespace gearoenix::metal {
inline vector_float4 simd_make_float4(const math::Vec4<float>& v) noexcept {
    return ::simd_make_float4(v.x, v.y, v.z, v.w);
}

inline vector_float4 simd_make_float4(const math::Vec3<float>& v, const float w) noexcept {
    return ::simd_make_float4(v.x, v.y, v.z, w);
}

inline vector_float4 simd_make_float4(const float x, const float y, const float z, const float w) noexcept {
    return ::simd_make_float4(x, y, z, w);
}
}

#endif
#endif

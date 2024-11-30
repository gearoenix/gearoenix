#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../math/gx-math-matrix-4d.hpp"
#import "../math/gx-math-vector-4d.hpp"
#import <simd/simd.h>

namespace gearoenix::metal {
template <typename T>
inline matrix_float4x4 simd_make_float4x4_t(const math::Mat4x4<T>& v)
{
    return ::simd_matrix(
        ::simd_make_float4(
            static_cast<float>(v.data[0][0]),
            static_cast<float>(v.data[0][1]),
            static_cast<float>(v.data[0][2]),
            static_cast<float>(v.data[0][3])),
        ::simd_make_float4(
            static_cast<float>(v.data[1][0]),
            static_cast<float>(v.data[1][1]),
            static_cast<float>(v.data[1][2]),
            static_cast<float>(v.data[1][3])),
        ::simd_make_float4(
            static_cast<float>(v.data[2][0]),
            static_cast<float>(v.data[2][1]),
            static_cast<float>(v.data[2][2]),
            static_cast<float>(v.data[2][3])),
        ::simd_make_float4(
            static_cast<float>(v.data[3][0]),
            static_cast<float>(v.data[3][1]),
            static_cast<float>(v.data[3][2]),
            static_cast<float>(v.data[3][3])));
}

inline vector_float4 simd_make_float4(const math::Vec4<float>& v)
{
    return ::simd_make_float4(v.x, v.y, v.z, v.w);
}

inline vector_float4 simd_make_float4(const math::Vec3<float>& v, const float w)
{
    return ::simd_make_float4(v.x, v.y, v.z, w);
}

inline vector_float4 simd_make_float4(const float x, const float y, const float z, const float w)
{
    return ::simd_make_float4(x, y, z, w);
}
}

#endif
#ifndef GEAROENIX_RENDER_BUFFER_UNIFORM_STRUCT_HPP // no pragma, because of some of the shader compilers.
#define GEAROENIX_RENDER_BUFFER_UNIFORM_STRUCT_HPP

#include "../gx-rnd-constants.hpp"

#ifdef __cplusplus
#include "../../core/macro/gx-cr-mcr-struct.hpp"
#include "../../math/gx-math-matrix-4d.hpp"

using vec2 = gearoenix::math::Vec2<float>;
using vec3 = gearoenix::math::Vec3<float>;
using vec4 = gearoenix::math::Vec4<float>;
using mat4 = gearoenix::math::Mat4x4<float>;
using uint = std::uint32_t;
using uvec4 = gearoenix::math::Vec4<std::uint32_t>;

#define GX_UNIFORM_STRUCT_DEF(name, body)    \
    GX_STRUCT_DEF(GxShaderData##name, body); \
    static_assert(sizeof(GxShaderData##name) % 16 == 0, "Uniform struct size must be multiple of 16 bytes")

#else // it means in shader

#define GX_UNIFORM_STRUCT_DEF(name, body) struct GxShaderData##name body

#endif

#ifndef GX_PI
#define GX_PI 3.141592653589793238
#endif

#ifndef GX_INVALID_INDEX32
#define GX_INVALID_INDEX32 0xFFFFFFFF
#endif

#ifndef GX_INVALID_INDEX16
#define GX_INVALID_INDEX16 0xFFFF
#endif

#endif
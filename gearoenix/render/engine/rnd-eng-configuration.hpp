#ifndef GEAROENIX_RENDER_ENGINE_CONFIGURATION_HPP
#define GEAROENIX_RENDER_ENGINE_CONFIGURATION_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-static.hpp"
#include "../../math/math-numeric.hpp"
#include "../../system/sys-log.hpp"
#include <cstdint>

namespace gearoenix::render::engine {
struct Configuration {
    GX_GETSET_VAL_PRV(std::int8_t, shadow_cascades_count, GX_MAX_SHADOW_CASCADES)
    GX_GETSET_VAL_PRV(std::uint16_t, runtime_reflection_environment_resolution, GX_DEFAULT_RUNTIME_REFLECTION_ENVIRONMENT_RESOLUTION)
    GX_GETSET_VAL_PRV(std::uint16_t, runtime_reflection_irradiance_resolution, GX_DEFAULT_RUNTIME_REFLECTION_IRRADIANCE_RESOLUTION)
    GX_GET_VAL_PRV(std::uint16_t, runtime_reflection_radiance_resolution, GX_DEFAULT_RUNTIME_REFLECTION_RADIANCE_RESOLUTION)
    GX_GET_VAL_PRV(std::uint8_t, runtime_reflection_radiance_levels, 1)
    GX_GETSET_VAL_PRV(std::uint16_t, brdflut_resolution, GX_DEFAULT_BRDFLUT_RESOLUTION)

    constexpr Configuration() noexcept
    {
        set_runtime_reflection_radiance_resolution(runtime_reflection_radiance_resolution);
    }

    constexpr void set_runtime_reflection_radiance_resolution(const std::uint16_t r) noexcept
    {
        runtime_reflection_radiance_resolution = r;
        runtime_reflection_radiance_levels = static_cast<std::uint8_t>(math::Numeric::floor_log2(r) - 2);
    }

    static int compute_runtime_reflection_radiance_levels(const int r) noexcept
    {
#ifdef GX_DEBUG_MODE
        if (r <= 0) {
            GXLOGF("Resolution(" << r << ") can not be equal or less than zero.")
        }
#endif
        const auto l = math::Numeric::floor_log2(r);
#ifdef GX_DEBUG_MODE
        if (l <= 2) {
            GXLOGF("Logarithm of 2 of Resolution(" << r << ") = " << l << " can not be equal or less than 2.")
        }
#endif
        return l - 2;
    }
};
}
#endif
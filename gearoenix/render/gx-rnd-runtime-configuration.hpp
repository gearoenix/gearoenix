#ifndef GEAROENIX_RENDER_RUNTIME_CONFIGURATION_HPP
#define GEAROENIX_RENDER_RUNTIME_CONFIGURATION_HPP

#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-rnd-build-configuration.hpp"
#include <cstdint>

namespace gearoenix::render {
struct RuntimeConfiguration {
    GX_GETSET_VAL_PRV(bool, vulkan_render_backend_enabled, true)
    GX_GETSET_VAL_PRV(bool, direct3dx_render_backend_enabled, true)
    GX_GETSET_VAL_PRV(bool, metal_render_backend_enabled, true)
    GX_GETSET_VAL_PRV(bool, opengl_render_backend_enabled, true)
    GX_GETSET_VAL_PRV(std::int8_t, shadow_cascades_count, GX_RENDER_MAX_SHADOW_CASCADES)
    GX_GETSET_VAL_PRV(std::uint16_t, runtime_reflection_environment_resolution, GX_RENDER_DEFAULT_RUNTIME_REFLECTION_ENVIRONMENT_RESOLUTION)
    GX_GETSET_VAL_PRV(std::uint16_t, runtime_reflection_irradiance_resolution, GX_RENDER_DEFAULT_RUNTIME_REFLECTION_IRRADIANCE_RESOLUTION)
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_cpu_render_memory_size, 512 * 1024 * 1024)
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_gpu_render_memory_size, 256 * 1024 * 1024)
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_gpu_buffer_size, 64 * 1024 * 1024)
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_dynamic_buffer_size, maximum_gpu_buffer_size >> 3u)
    GX_GETSET_VAL_PRV(std::uint16_t, brdflut_resolution, GX_RENDER_DEFAULT_BRDFLUT_RESOLUTION)
    GX_GET_VAL_PRV(std::uint16_t, runtime_reflection_radiance_resolution, GX_RENDER_DEFAULT_RUNTIME_REFLECTION_RADIANCE_RESOLUTION)
    GX_GET_VAL_PRV(std::uint8_t, runtime_reflection_radiance_levels, 1)
    GX_GETSET_VAL_PRV(std::uint32_t, runtime_resolution_width, GX_RENDER_DEFAULT_RESOLUTION_WIDTH)
    GX_GETSET_VAL_PRV(std::uint32_t, runtime_resolution_height, GX_RENDER_DEFAULT_RESOLUTION_HEIGHT)

    RuntimeConfiguration() noexcept;
    void set_runtime_reflection_radiance_resolution(std::uint16_t value) noexcept;
};
}
#endif
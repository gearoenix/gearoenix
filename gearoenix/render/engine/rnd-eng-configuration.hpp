#ifndef GEAROENIX_RENDER_ENGINE_CONFIGURATION_HPP
#define GEAROENIX_RENDER_ENGINE_CONFIGURATION_HPP
#include "../../core/cr-build-configuration.hpp"
#include <cstdint>

namespace gearoenix::render::engine {
struct Configuration {
    std::int8_t shadow_cascades_count = GX_MAX_SHADOW_CASCADES;
    std::uint16_t runtime_reflection_environment_resolution = GX_MAX_RUNTIME_REFLECTION_ENVIRONMENT_RESOLUTION;
    std::uint16_t runtime_reflection_irradiance_resolution = GX_MAX_RUNTIME_REFLECTION_IRRADIANCE_RESOLUTION;
    std::uint16_t runtime_reflection_radiance_resolution = GX_MAX_RUNTIME_REFLECTION_RADIANCE_RESOLUTION;
    std::uint8_t runtime_reflection_radiance_levels = GX_MAX_RUNTIME_REFLECTION_RADIANCE_LEVELS;
};
}
#endif
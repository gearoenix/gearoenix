#include "gx-rnd-runtime-configuration.hpp"
#include "../math/gx-math-numeric.hpp"

gearoenix::render::RuntimeConfiguration::RuntimeConfiguration() noexcept
{
    set_runtime_reflection_radiance_resolution(runtime_reflection_radiance_resolution);
}

void gearoenix::render::RuntimeConfiguration::set_runtime_reflection_radiance_resolution(const std::uint16_t r) noexcept
{
    runtime_reflection_radiance_resolution = r;
    runtime_reflection_radiance_levels = compute_radiance_mipmaps_count(r);
}

std::uint8_t gearoenix::render::RuntimeConfiguration::compute_radiance_mipmaps_count(std::uint16_t value) noexcept
{
    return static_cast<std::uint8_t>(math::Numeric::floor_log2(value) - 4);
}

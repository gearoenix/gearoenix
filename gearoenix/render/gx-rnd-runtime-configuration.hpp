#ifndef GEAROENIX_RENDER_RUNTIME_CONFIGURATION_HPP
#define GEAROENIX_RENDER_RUNTIME_CONFIGURATION_HPP
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../core/gx-cr-observed.hpp"
#include "gx-rnd-build-configuration.hpp"
#include "gx-rnd-resolution.hpp"
#include <cstdint>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render {
struct RuntimeConfiguration final : core::ecs::Component {
    GX_GETSET_VAL_PRV(std::uint8_t, shadow_cascades_count, GX_RENDER_MAX_SHADOW_CASCADES);
    GX_GETSET_VAL_PRV(std::uint16_t, runtime_reflection_environment_resolution, GX_RENDER_DEFAULT_RUNTIME_REFLECTION_ENVIRONMENT_RESOLUTION);
    GX_GETSET_VAL_PRV(std::uint16_t, runtime_reflection_irradiance_resolution, GX_RENDER_DEFAULT_RUNTIME_REFLECTION_IRRADIANCE_RESOLUTION);
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_cpu_render_memory_size, 512 * 1024 * 1024);
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_gpu_render_memory_size, 256 * 1024 * 1024);
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_gpu_buffer_size, 64 * 1024 * 1024);
    GX_GETSET_VAL_PRV(std::uint32_t, maximum_dynamic_buffer_size, maximum_gpu_buffer_size >> 3u);
    GX_GETSET_VAL_PRV(std::uint16_t, brdflut_resolution, GX_RENDER_DEFAULT_BRDFLUT_RESOLUTION);
    GX_GET_VAL_PRV(std::uint16_t, runtime_reflection_radiance_resolution, GX_RENDER_DEFAULT_RUNTIME_REFLECTION_RADIANCE_RESOLUTION);
    GX_GET_VAL_PRV(std::uint8_t, runtime_reflection_radiance_levels, 1);
    GX_GET_REF_PRV(core::Observed<Resolution>, runtime_resolution);

    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;
    explicit RuntimeConfiguration(core::ecs::entity_id_t);

public:
    RuntimeConfiguration(const RuntimeConfiguration&) = delete;
    RuntimeConfiguration(RuntimeConfiguration&&) = delete;
    [[nodiscard]] static RuntimeConfiguration& get(core::ecs::World* w);
    [[nodiscard]] static RuntimeConfiguration& get(engine::Engine* e);
    [[nodiscard]] static RuntimeConfiguration& get(engine::Engine& e);
    ~RuntimeConfiguration() override;
    void set_runtime_reflection_radiance_resolution(std::uint16_t value);
    [[nodiscard]] static std::uint8_t compute_radiance_mipmaps_count(std::uint16_t value);
    void show_debug_gui(const engine::Engine&) override;
};
}
#endif
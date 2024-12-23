#pragma once
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
    constexpr static std::uint32_t MAX_COUNT = 1;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 25;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

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

    void write_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const override;
    void update_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) override;

public:
    explicit RuntimeConfiguration(core::ecs::entity_id_t);
    RuntimeConfiguration(const RuntimeConfiguration&) = delete;
    RuntimeConfiguration(RuntimeConfiguration&&) = delete;
    ~RuntimeConfiguration() override;
    void set_runtime_reflection_radiance_resolution(std::uint16_t value);
    [[nodiscard]] static std::uint8_t compute_radiance_mipmaps_count(std::uint16_t value);
    void show_debug_gui() override;
};
}
#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "gx-vk-rfl-probe.hpp"

namespace gearoenix::vulkan::texture {
struct Target;
struct TextureCube;
}

namespace gearoenix::vulkan::reflection {
struct Runtime final : render::reflection::Runtime, Probe {
    GEAROENIX_OBJECT_STRUCT_DEF;

    using GapiCubeTarget = std::array<std::shared_ptr<texture::Target>, 6>;
    using GapiMippedCubeTarget = std::array<boost::container::static_vector<std::shared_ptr<texture::Target>, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6>;

    constexpr static auto max_count = render::reflection::Runtime::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_reflection_runtime_type_index;
    constexpr static std::array all_parent_object_type_indices {
        render::reflection::Runtime::object_type_index,
        reflection::Probe::object_type_index,
        render::reflection::Probe::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices {
        render::reflection::Runtime::object_type_index,
        reflection::Probe::object_type_index };

    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, gapi_environment);
    GX_GET_CREF_PRV(GapiCubeTarget, gapi_environment_targets);
    GX_GET_CREF_PRV(GapiCubeTarget, gapi_irradiance_targets);
    GX_GET_CREF_PRV(GapiMippedCubeTarget, gapi_radiance_targets);

    GX_GET_VAL_PRV(std::uint32_t, environment_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, environment_sampler_index, static_cast<std::uint32_t>(-1));

    void initialise_gapi();

    Runtime(
        core::ecs::Entity* entity,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name);

public:
    static void construct(
        core::ecs::Entity* entity,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCallerShared<Runtime>&& end_callback);
    ~Runtime() override;
};
}
#endif
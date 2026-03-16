#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-rfl-probe.hpp"

#include <array>

namespace gearoenix::vulkan::texture {
struct Target;
struct TextureCube;
}

namespace gearoenix::vulkan::reflection {
struct Runtime final : render::reflection::Runtime, Probe {
    GEAROENIX_OBJECT_STRUCT_DEF;

    struct IrradiancePushConstants final {
        float u_axis[4];
        float v_axis[4];
        float face_axis[3];
        std::uint32_t image_size;
    };

    struct RadiancePushConstants final {
        float u_axis[4];
        float v_axis[4];
        float face_axis[3];
        std::uint32_t image_size;
        float roughness;
        float roughness_p_4;
        float sa_texel;
    };

    using GapiCubeTarget = std::array<std::shared_ptr<texture::Target>, 6>;
    using GapiMippedCubeTarget = std::array<boost::container::static_vector<std::shared_ptr<texture::Target>, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6>;
    using CubeDescriptorSet = std::array<VkDescriptorSet, 6>;
    using MippedCubeDescriptorSet = std::array<boost::container::static_vector<VkDescriptorSet, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6>;

    constexpr static auto max_count = render::reflection::Runtime::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_reflection_runtime_type_index;
    constexpr static std::array all_parent_object_type_indices {
        render::reflection::Runtime::object_type_index,
        reflection::Probe::object_type_index,
        render::reflection::Probe::object_type_index
    };
    constexpr static std::array immediate_parent_object_type_indices {
        render::reflection::Runtime::object_type_index,
        reflection::Probe::object_type_index
    };

    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, gapi_environment);
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, gapi_irradiance);
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, gapi_radiance);

    GX_GET_CREF_PRV(GapiCubeTarget, gapi_environment_targets);
    GX_GET_CREF_PRV(GapiCubeTarget, gapi_irradiance_targets);
    GX_GET_CREF_PRV(GapiMippedCubeTarget, gapi_radiance_targets);

    GX_GET_VAL_PRV(VkDescriptorPool, irradiance_descriptor_pool, nullptr);
    GX_GET_CREF_PRV(CubeDescriptorSet, irradiance_descriptor_sets);

    GX_GET_VAL_PRV(VkDescriptorPool, radiance_descriptor_pool, nullptr);
    GX_GET_CREF_PRV(MippedCubeDescriptorSet, radiance_descriptor_sets);

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
    void convolute_irradiance(VkCommandBuffer cmd) const;
    void convolute_radiance(VkCommandBuffer cmd) const;
    void vk_update(VkCommandBuffer cmd) const;
};
}
#endif
#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"

namespace gearoenix::vulkan::texture {
struct TextureCube;
}

namespace gearoenix::vulkan::reflection {
struct Probe {
    constexpr static auto max_count = render::reflection::Probe::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_reflection_probe_type_index;
    constexpr static auto all_parent_object_type_indices = core::Object::all_parent_object_type_indices;
    constexpr static auto immediate_parent_object_type_indices = core::Object::immediate_parent_object_type_indices;

    GX_GET_VAL_PRT(std::uint32_t, irradiance_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRT(std::uint32_t, irradiance_sampler_index, static_cast<std::uint32_t>(-1));

    GX_GET_VAL_PRT(std::uint32_t, radiance_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRT(std::uint32_t, radiance_sampler_index, static_cast<std::uint32_t>(-1));

    void set_textures(const texture::TextureCube& irr, const texture::TextureCube& rad);

public:
    virtual ~Probe();
};
}
#endif
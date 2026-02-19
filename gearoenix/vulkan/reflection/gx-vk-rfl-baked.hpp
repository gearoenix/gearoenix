#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-baked.hpp"
#include "gx-vk-rfl-probe.hpp"

namespace gearoenix::vulkan::reflection {
struct Baked final : Probe, render::reflection::Baked {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = render::reflection::Baked::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_reflection_runtime_type_index;
    constexpr static std::array all_parent_object_type_indices {
        render::reflection::Baked::object_type_index,
        vulkan::reflection::Probe::object_type_index,
        render::reflection::Probe::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices {
        render::reflection::Baked::object_type_index,
        vulkan::reflection::Probe::object_type_index };

    Baked(
        core::ecs::Entity* entity,
        std::string&& name,
        std::shared_ptr<render::texture::TextureCube>&& irr,
        std::shared_ptr<render::texture::TextureCube>&& rad,
        const math::Aabb3<double>& include_box);
    ~Baked() override;
};
}
#endif
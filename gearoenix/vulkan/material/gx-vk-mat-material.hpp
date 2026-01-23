#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-object-type-indices.hpp"

#include <array>

namespace gearoenix::vulkan::pipeline {
struct Pipeline;
}

namespace gearoenix::vulkan::material {
struct Material {
    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_gapi_material_type_index;
    constexpr static std::array<core::object_type_index_t, 0> all_parent_object_type_indices {};
    constexpr static std::array<core::object_type_index_t, 0> immediate_parent_object_type_indices {};

    virtual ~Material();
    virtual void bind_forward(pipeline::Pipeline*& pipeline);
    virtual void bind_shadow(pipeline::Pipeline*& pipeline);
};
}

#endif
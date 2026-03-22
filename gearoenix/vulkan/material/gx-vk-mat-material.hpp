#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-object-type-indices.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mat-manager.hpp"

#include <array>

struct GxShaderDataMaterial;

namespace gearoenix::vulkan::pipeline {
struct FormatPipelines;
struct PushConstants;
}

namespace gearoenix::vulkan::material {
struct Material {
    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_gapi_material_type_index;
    constexpr static std::array<core::object_type_index_t, 0> all_parent_object_type_indices { };
    constexpr static std::array<core::object_type_index_t, 0> immediate_parent_object_type_indices { };

protected:
    uniform_indexer_t::DataAccess shader_data;

    Material();

public:
    Material(Material&&) = delete;
    Material(const Material&) = delete;

    virtual ~Material();
    virtual void bind_forward(vk::CommandBuffer cmd, bool skinned, const pipeline::FormatPipelines& fp, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline);
    virtual void bind_shadow(vk::CommandBuffer cmd, bool skinned, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline);
    void bind_graphics(vk::Pipeline pipeline, vk::CommandBuffer cmd, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline);
};
}

#endif
#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "mesh/gx-vk-msh-draw-cache.hpp"
#include "pipeline/gx-vk-pip-format-pipelines.hpp"
#include "pipeline/gx-vk-pip-push-constant.hpp"

namespace gearoenix::render::record {
struct CameraModel;
}

namespace gearoenix::vulkan {
struct DrawState final {
    constexpr static std::uint8_t INVALID_PIPELINE_INDEX = 0xFF;

    pipeline::PushConstants push_constants;
    vk::CommandBuffer command_buffer = nullptr;
    std::uint8_t current_pipeline_index = INVALID_PIPELINE_INDEX;
    vk::Buffer gpu_buffer = nullptr;
    vk::PipelineLayout bindless_pipeline_layout = nullptr;
    pipeline::AllPipelines pipelines;
    vk::Format pipeline_target_format = vk::Format::eUndefined;
    const render::record::CameraModel* camera_model = nullptr;
};

inline void draw_mesh(DrawState& draw_state, const mesh::DrawCache& dc, const std::uint8_t pipeline_index)
{
    if (pipeline_index != draw_state.current_pipeline_index) {
        draw_state.command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, draw_state.pipelines[pipeline_index]);
        draw_state.current_pipeline_index = pipeline_index;
    }
    draw_state.push_constants.material_index = dc.material_draw_cache.material_index;
    draw_state.command_buffer.bindVertexBuffers(0, draw_state.gpu_buffer, dc.vertex_offset);
    draw_state.command_buffer.bindIndexBuffer(draw_state.gpu_buffer, dc.index_offset, vk::IndexType::eUint32);
    draw_state.command_buffer.pushConstants(
        draw_state.bindless_pipeline_layout, vk::ShaderStageFlagBits::eAll, 0u, sizeof(draw_state.push_constants), &draw_state.push_constants);
    draw_state.command_buffer.drawIndexed(dc.indices_count, 1, 0, 0, 0);
}
}
#endif

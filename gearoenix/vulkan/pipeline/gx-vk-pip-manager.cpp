#include "gx-vk-pip-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/gx-rnd-vertex.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../shader/gx-vk-shd-manager.hpp"
#include "../shader/gx-vk-shd-module.hpp"
#include "gx-vk-pip-cache.hpp"
#include "gx-vk-pip-pipeline.hpp"

namespace {
constexpr char default_stage_entry[] = "main";
constexpr auto fixed_depth_format = vk::Format::eD32Sfloat;

constexpr std::array dynamic_states = {
    vk::DynamicState::eViewport,
    vk::DynamicState::eScissor,
    vk::DynamicState::eFrontFace,
    vk::DynamicState::eCullMode,
};
}

// TODO: Separate ray tracing and rasterisation pipeline initialisation into distinct struct.
// TODO: Use specialisation constants to split monolithic shaders (e.g. unlit) into dedicated pipeline
//       variants per feature (alpha blending, UV transform, albedo source) instead of a single
//       uber-shader that handles all combinations at runtime.

void gearoenix::vulkan::pipeline::Manager::initialise_ray_tracing()
{
    ray_gen_sm = shader_manager->get("camera.rgen");
    close_hit_sm = shader_manager->get("pbr.rchit");
    shadow_miss_sm = shader_manager->get("pbr-shadow.rmiss");
    miss_sm = shader_manager->get("pbr-sky.rmiss");

    stages_create_info.resize(4);
    shader_group_create_info.resize(4);

    // Ray gen part -----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t ray_gen_shader_index = 0;

        auto& ray_gen_stage_create_info = stages_create_info[ray_gen_shader_index];
        ray_gen_stage_create_info.module = ray_gen_sm->get_vulkan_data();
        ray_gen_stage_create_info.stage = vk::ShaderStageFlagBits::eRaygenKHR;
        ray_gen_stage_create_info.pName = default_stage_entry;

        auto& ray_gen_sg = shader_group_create_info[ray_gen_shader_index];
        ray_gen_sg.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
        ray_gen_sg.anyHitShader = vk::ShaderUnusedKHR;
        ray_gen_sg.closestHitShader = vk::ShaderUnusedKHR;
        ray_gen_sg.generalShader = ray_gen_shader_index;
        ray_gen_sg.intersectionShader = vk::ShaderUnusedKHR;
    }

    // Miss sky part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t miss_sky_shader_index = 1;

        auto& miss_sky_stage_create_info = stages_create_info[miss_sky_shader_index];
        miss_sky_stage_create_info.module = miss_sm->get_vulkan_data();
        miss_sky_stage_create_info.stage = vk::ShaderStageFlagBits::eMissKHR;
        miss_sky_stage_create_info.pName = default_stage_entry;

        auto& miss_sky_sg = shader_group_create_info[miss_sky_shader_index];
        miss_sky_sg.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
        miss_sky_sg.anyHitShader = vk::ShaderUnusedKHR;
        miss_sky_sg.closestHitShader = vk::ShaderUnusedKHR;
        miss_sky_sg.generalShader = miss_sky_shader_index;
        miss_sky_sg.intersectionShader = vk::ShaderUnusedKHR;
    }

    // Miss shadow part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t miss_shadow_shader_index = 2;

        auto& miss_shadow_stage_create_info = stages_create_info[miss_shadow_shader_index];
        miss_shadow_stage_create_info.module = miss_sm->get_vulkan_data();
        miss_shadow_stage_create_info.stage = vk::ShaderStageFlagBits::eMissKHR;
        miss_shadow_stage_create_info.pName = default_stage_entry;

        auto& miss_shadow_sg = shader_group_create_info[miss_shadow_shader_index];
        miss_shadow_sg.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
        miss_shadow_sg.anyHitShader = vk::ShaderUnusedKHR;
        miss_shadow_sg.closestHitShader = vk::ShaderUnusedKHR;
        miss_shadow_sg.generalShader = miss_shadow_shader_index;
        miss_shadow_sg.intersectionShader = vk::ShaderUnusedKHR;
    }

    // Close hit part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t close_hit_shader_index = 3;

        auto& close_hit_stage_create_info = stages_create_info[close_hit_shader_index];
        close_hit_stage_create_info.module = close_hit_sm->get_vulkan_data();
        close_hit_stage_create_info.stage = vk::ShaderStageFlagBits::eClosestHitKHR;
        close_hit_stage_create_info.pName = default_stage_entry;

        auto& close_hit_sg = shader_group_create_info[close_hit_shader_index];
        close_hit_sg.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
        close_hit_sg.anyHitShader = vk::ShaderUnusedKHR;
        close_hit_sg.closestHitShader = close_hit_shader_index;
        close_hit_sg.generalShader = vk::ShaderUnusedKHR;
        close_hit_sg.intersectionShader = vk::ShaderUnusedKHR;
    }
}

void gearoenix::vulkan::pipeline::Manager::load_shaders()
{
    pbr_vert_sm = shader_manager->get("forward-pbr.vert");
    pbr_frag_sm = shader_manager->get("forward-pbr.frag");
    unlit_vert_sm = shader_manager->get("forward-unlit.vert");
    unlit_frag_sm = shader_manager->get("forward-unlit.frag");
    skybox_vert_sm = shader_manager->get("skybox.vert");
    skybox_equirectangular_frag_sm = shader_manager->get("skybox-equirectangular.frag");
    skybox_cube_frag_sm = shader_manager->get("skybox-cube.frag");
    shadow_caster_vert_sm = shader_manager->get("shadow-caster.vert");
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_pbr_forward_pipeline(const vk::Format colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    std::array<vk::PipelineShaderStageCreateInfo, 2> stages { };
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = pbr_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = pbr_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertex), vk::VertexInputRate::eVertex };

    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, normal) },
        vk::VertexInputAttributeDescription { 2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertex, tangent) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
        vk::VertexInputAttributeDescription { 4, 0, vk::Format::eR32G32B32A32Sfloat, 0 }, // dummy bone_weights
        vk::VertexInputAttributeDescription { 5, 0, vk::Format::eR32G32B32A32Sfloat, 0 }, // dummy bone_indices
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = true;
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendAttachmentState colour_blend_attachment;
    colour_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colour_blend;
    colour_blend.setAttachments(colour_blend_attachment);

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.setColorAttachmentFormats(colour_format);
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(stages);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_pbr_skinned_forward_pipeline(const vk::Format colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    constexpr vk::Bool32 spec_has_bones = true;
    constexpr vk::SpecializationMapEntry spec_map_entry { 0, 0, sizeof(vk::Bool32) };
    vk::SpecializationInfo spec_info;
    spec_info.setMapEntries(spec_map_entry).setData<vk::Bool32>(spec_has_bones);

    std::array<vk::PipelineShaderStageCreateInfo, 2> stages { };
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = pbr_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[0].pSpecializationInfo = &spec_info;
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = pbr_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertexAnimated), vk::VertexInputRate::eVertex };

    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, normal) },
        vk::VertexInputAttributeDescription { 2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertex, tangent) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
        vk::VertexInputAttributeDescription { 4, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertexAnimated, bone_weights) },
        vk::VertexInputAttributeDescription { 5, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertexAnimated, bone_indices) },
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = true;
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendAttachmentState colour_blend_attachment;
    colour_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colour_blend;
    colour_blend.setAttachments(colour_blend_attachment);

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.setColorAttachmentFormats(colour_format);
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(stages);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_unlit_forward_pipeline(const vk::Format colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    std::array<vk::PipelineShaderStageCreateInfo, 2> stages { };
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = unlit_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = unlit_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertex), vk::VertexInputRate::eVertex };

    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, normal) },
        vk::VertexInputAttributeDescription { 2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertex, tangent) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
        vk::VertexInputAttributeDescription { 4, 0, vk::Format::eR32G32B32A32Sfloat, 0 }, // dummy bone_weights
        vk::VertexInputAttributeDescription { 5, 0, vk::Format::eR32G32B32A32Sfloat, 0 }, // dummy bone_indices
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = true;
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendAttachmentState colour_blend_attachment;
    colour_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colour_blend;
    colour_blend.setAttachments(colour_blend_attachment);

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.setColorAttachmentFormats(colour_format);
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(stages);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_unlit_skinned_forward_pipeline(const vk::Format colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    constexpr vk::Bool32 spec_has_bones = true;
    constexpr vk::SpecializationMapEntry spec_map_entry { 0, 0, sizeof(vk::Bool32) };
    vk::SpecializationInfo spec_info;
    spec_info.setMapEntries(spec_map_entry).setData<vk::Bool32>(spec_has_bones);

    std::array<vk::PipelineShaderStageCreateInfo, 2> stages { };
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = unlit_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[0].pSpecializationInfo = &spec_info;
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = unlit_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertexAnimated), vk::VertexInputRate::eVertex };

    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, normal) },
        vk::VertexInputAttributeDescription { 2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertex, tangent) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
        vk::VertexInputAttributeDescription { 4, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertexAnimated, bone_weights) },
        vk::VertexInputAttributeDescription { 5, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertexAnimated, bone_indices) },
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = true;
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendAttachmentState colour_blend_attachment;
    colour_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colour_blend;
    colour_blend.setAttachments(colour_blend_attachment);

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.setColorAttachmentFormats(colour_format);
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(stages);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_skybox_equirectangular_pipeline(const vk::Format colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    std::array<vk::PipelineShaderStageCreateInfo, 2> stages { };
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = skybox_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = skybox_equirectangular_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertex), vk::VertexInputRate::eVertex };

    // Skybox only uses locations 0-3 (position, normal, tangent, uv), no bone attributes
    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, normal) },
        vk::VertexInputAttributeDescription { 2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertex, tangent) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = false; // skybox does not write depth
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendAttachmentState colour_blend_attachment;
    colour_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colour_blend;
    colour_blend.setAttachments(colour_blend_attachment);

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.setColorAttachmentFormats(colour_format);
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(stages);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_skybox_cube_pipeline(const vk::Format colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    std::array<vk::PipelineShaderStageCreateInfo, 2> stages { };
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = skybox_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = skybox_cube_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertex), vk::VertexInputRate::eVertex };

    // Skybox only uses locations 0-3 (position, normal, tangent, uv), no bone attributes
    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, normal) },
        vk::VertexInputAttributeDescription { 2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertex, tangent) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = false; // skybox does not write depth
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendAttachmentState colour_blend_attachment;
    colour_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colour_blend;
    colour_blend.setAttachments(colour_blend_attachment);

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.setColorAttachmentFormats(colour_format);
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(stages);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_shadow_pipeline(const vk::Format depth_format)
{
    const auto& bindless = descriptor::Bindless::get();

    vk::PipelineShaderStageCreateInfo vert_stage;
    vert_stage.stage = vk::ShaderStageFlagBits::eVertex;
    vert_stage.module = shadow_caster_vert_sm->get_vulkan_data();
    vert_stage.pName = default_stage_entry;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertex), vk::VertexInputRate::eVertex };

    // Shadow caster uses position(0), uv(3), dummy bone_weights(4), dummy bone_indices(5)
    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
        vk::VertexInputAttributeDescription { 4, 0, vk::Format::eR32G32B32A32Sfloat, 0 }, // dummy bone_weights
        vk::VertexInputAttributeDescription { 5, 0, vk::Format::eR32G32B32A32Sfloat, 0 }, // dummy bone_indices
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = true;
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendStateCreateInfo colour_blend;

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.depthAttachmentFormat = depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(vert_stage);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_skinned_shadow_pipeline(const vk::Format depth_format)
{
    const auto& bindless = descriptor::Bindless::get();

    constexpr vk::Bool32 spec_has_bones = true;
    constexpr vk::SpecializationMapEntry spec_map_entry { 0, 0, sizeof(vk::Bool32) };
    vk::SpecializationInfo spec_info;
    spec_info.setMapEntries(spec_map_entry).setData<vk::Bool32>(spec_has_bones);

    vk::PipelineShaderStageCreateInfo vert_stage;
    vert_stage.stage = vk::ShaderStageFlagBits::eVertex;
    vert_stage.module = shadow_caster_vert_sm->get_vulkan_data();
    vert_stage.pName = default_stage_entry;
    vert_stage.pSpecializationInfo = &spec_info;

    constexpr vk::VertexInputBindingDescription vertex_binding { 0, sizeof(render::PbrVertexAnimated), vk::VertexInputRate::eVertex };

    constexpr std::array vertex_attributes {
        vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::PbrVertex, position) },
        vk::VertexInputAttributeDescription { 3, 0, vk::Format::eR32G32Sfloat, offsetof(render::PbrVertex, uv) },
        vk::VertexInputAttributeDescription { 4, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertexAnimated, bone_weights) },
        vk::VertexInputAttributeDescription { 5, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::PbrVertexAnimated, bone_indices) },
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.setVertexBindingDescriptions(vertex_binding);
    vertex_input_info.setVertexAttributeDescriptions(vertex_attributes);

    constexpr vk::PipelineInputAssemblyStateCreateInfo input_assembly { { }, vk::PrimitiveTopology::eTriangleList };

    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_states);

    vk::PipelineViewportStateCreateInfo viewport_state;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterization;
    rasterization.polygonMode = vk::PolygonMode::eFill;
    rasterization.cullMode = vk::CullModeFlagBits::eBack;
    rasterization.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil;
    depth_stencil.depthTestEnable = true;
    depth_stencil.depthWriteEnable = true;
    depth_stencil.depthCompareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineColorBlendStateCreateInfo colour_blend;

    vk::PipelineRenderingCreateInfo rendering_info;
    rendering_info.depthAttachmentFormat = depth_format;

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.pNext = &rendering_info;
    create_info.setStages(vert_stage);
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &colour_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    return Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

gearoenix::vulkan::pipeline::Manager::Manager()
    : Singleton(this)
    , cache(new Cache())
    , shader_manager(new shader::Manager())
{
    if (device::Physical::get().get_rtx_supported()) {
        initialise_ray_tracing();
    }
    load_shaders();
    initialise_shadow_pipelines();
}

gearoenix::vulkan::pipeline::Manager::~Manager() = default;

void gearoenix::vulkan::pipeline::Manager::initialise_shadow_pipelines()
{
    shadow = create_shadow_pipeline(fixed_depth_format);
    skinned_shadow = create_skinned_shadow_pipeline(fixed_depth_format);
}

gearoenix::vulkan::pipeline::FormatPipelinesDataHolder gearoenix::vulkan::pipeline::Manager::create_format_pipelines(const vk::Format colour_format)
{
    GX_ASSERT_D(colour_format != vk::Format::eUndefined);

    FormatPipelinesDataHolder fp;

    fp.pbr_forward = create_pbr_forward_pipeline(colour_format);
    fp.pbr_skinned_forward = create_pbr_skinned_forward_pipeline(colour_format);
    fp.unlit_forward = create_unlit_forward_pipeline(colour_format);
    fp.unlit_skinned_forward = create_unlit_skinned_forward_pipeline(colour_format);
    fp.skybox_equirectangular = create_skybox_equirectangular_pipeline(colour_format);
    fp.skybox_cube = create_skybox_cube_pipeline(colour_format);

    return fp;
}

void gearoenix::vulkan::pipeline::Manager::set(AllPipelines& pipelines, const vk::Format colour_format)
{
    auto& fp = formats_pipelines[colour_format];
    if (nullptr == fp.pbr_forward) {
        fp = create_format_pipelines(colour_format);
    }

    pipelines[static_cast<std::size_t>(FormatPipelinesIndices::pbr_forward_index)] = fp.pbr_forward->get_vulkan_data();
    pipelines[static_cast<std::size_t>(FormatPipelinesIndices::pbr_skinned_forward_index)] = fp.pbr_skinned_forward->get_vulkan_data();
    pipelines[static_cast<std::size_t>(FormatPipelinesIndices::unlit_forward_index)] = fp.unlit_forward->get_vulkan_data();
    pipelines[static_cast<std::size_t>(FormatPipelinesIndices::unlit_skinned_forward_index)] = fp.unlit_skinned_forward->get_vulkan_data();
    pipelines[static_cast<std::size_t>(FormatPipelinesIndices::skybox_equirectangular_index)] = fp.skybox_equirectangular->get_vulkan_data();
    pipelines[static_cast<std::size_t>(FormatPipelinesIndices::skybox_cube_index)] = fp.skybox_cube->get_vulkan_data();
}

void gearoenix::vulkan::pipeline::Manager::set(AllPipelines& pipelines) const
{
    pipelines[static_cast<std::size_t>(ShadowPipelinesIndices::no_skin_index)] = shadow->get_vulkan_data();
    pipelines[static_cast<std::size_t>(ShadowPipelinesIndices::skinned_index)] = skinned_shadow->get_vulkan_data();
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_ray_tracing_pbr(const std::shared_ptr<descriptor::SetLayout>&)
{
    GX_UNIMPLEMENTED;
}

#endif

#include "gx-vk-pip-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
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
constexpr VkFormat fixed_depth_format = VK_FORMAT_D32_SFLOAT;

constexpr std::array dynamic_states = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_FRONT_FACE,
    VK_DYNAMIC_STATE_CULL_MODE,
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
    GX_SET_VECTOR_ZERO(stages_create_info);
    shader_group_create_info.resize(4);
    GX_SET_VECTOR_ZERO(shader_group_create_info);

    // Ray gen part -----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t ray_gen_shader_index = 0;

        auto& ray_gen_stage_create_info = stages_create_info[ray_gen_shader_index];
        ray_gen_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ray_gen_stage_create_info.module = ray_gen_sm->get_vulkan_data();
        ray_gen_stage_create_info.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        ray_gen_stage_create_info.pName = default_stage_entry;

        auto& ray_gen_sg = shader_group_create_info[ray_gen_shader_index];
        ray_gen_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        ray_gen_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        ray_gen_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        ray_gen_sg.closestHitShader = VK_SHADER_UNUSED_KHR;
        ray_gen_sg.generalShader = ray_gen_shader_index;
        ray_gen_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }

    // Miss sky part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t miss_sky_shader_index = 1;

        auto& miss_sky_stage_create_info = stages_create_info[miss_sky_shader_index];
        miss_sky_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        miss_sky_stage_create_info.module = miss_sm->get_vulkan_data();
        miss_sky_stage_create_info.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
        miss_sky_stage_create_info.pName = default_stage_entry;

        auto& miss_sky_sg = shader_group_create_info[miss_sky_shader_index];
        miss_sky_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        miss_sky_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        miss_sky_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        miss_sky_sg.closestHitShader = VK_SHADER_UNUSED_KHR;
        miss_sky_sg.generalShader = miss_sky_shader_index;
        miss_sky_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }

    // Miss shadow part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t miss_shadow_shader_index = 2;

        auto& miss_shadow_stage_create_info = stages_create_info[miss_shadow_shader_index];
        miss_shadow_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        miss_shadow_stage_create_info.module = miss_sm->get_vulkan_data();
        miss_shadow_stage_create_info.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
        miss_shadow_stage_create_info.pName = default_stage_entry;

        auto& miss_shadow_sg = shader_group_create_info[miss_shadow_shader_index];
        miss_shadow_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        miss_shadow_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        miss_shadow_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        miss_shadow_sg.closestHitShader = VK_SHADER_UNUSED_KHR;
        miss_shadow_sg.generalShader = miss_shadow_shader_index;
        miss_shadow_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }

    // Close hit part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t close_hit_shader_index = 3;

        auto& close_hit_stage_create_info = stages_create_info[close_hit_shader_index];
        close_hit_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        close_hit_stage_create_info.module = close_hit_sm->get_vulkan_data();
        close_hit_stage_create_info.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        close_hit_stage_create_info.pName = default_stage_entry;

        auto& close_hit_sg = shader_group_create_info[close_hit_shader_index];
        close_hit_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        close_hit_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        close_hit_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        close_hit_sg.closestHitShader = close_hit_shader_index;
        close_hit_sg.generalShader = VK_SHADER_UNUSED_KHR;
        close_hit_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }
}

void gearoenix::vulkan::pipeline::Manager::load_shaders()
{
    pbr_vert_sm = shader_manager->get("forward-pbr.vert");
    pbr_frag_sm = shader_manager->get("forward-pbr.frag");
    unlit_vert_sm = shader_manager->get("forward-unlit.vert");
    unlit_frag_sm = shader_manager->get("forward-unlit.frag");
    skybox_equirectangular_vert_sm = shader_manager->get("skybox-equirectangular.vert");
    skybox_equirectangular_frag_sm = shader_manager->get("skybox-equirectangular.frag");
    shadow_caster_vert_sm = shader_manager->get("shadow-caster.vert");
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_pbr_forward_pipeline(const VkFormat colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    std::array<VkPipelineShaderStageCreateInfo, 2> stages {};
    GX_SET_ZERO(stages);
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = pbr_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = pbr_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertex);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 6> vertex_attributes {};
    GX_SET_ZERO(vertex_attributes);
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, normal);
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 2;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = offsetof(render::PbrVertex, tangent);
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 3;
    vertex_attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[3].offset = offsetof(render::PbrVertex, uv);
    vertex_attributes[4].binding = 0;
    vertex_attributes[4].location = 4;
    vertex_attributes[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[4].offset = 0; // dummy bone_weights
    vertex_attributes[5].binding = 0;
    vertex_attributes[5].location = 5;
    vertex_attributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[5].offset = 0; // dummy bone_indices

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendAttachmentState colour_blend_attachment;
    GX_SET_ZERO(colour_blend_attachment);
    colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colour_blend;
    GX_SET_ZERO(colour_blend);
    colour_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blend.attachmentCount = 1;
    colour_blend.pAttachments = &colour_blend_attachment;

    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &colour_format;
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    VkGraphicsPipelineCreateInfo create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = &rendering_info;
    create_info.stageCount = static_cast<std::uint32_t>(stages.size());
    create_info.pStages = stages.data();
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

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_pbr_skinned_forward_pipeline(const VkFormat colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    constexpr VkBool32 spec_has_bones = VK_TRUE;
    VkSpecializationMapEntry spec_map_entry;
    GX_SET_ZERO(spec_map_entry);
    spec_map_entry.constantID = 0;
    spec_map_entry.size = sizeof(VkBool32);
    VkSpecializationInfo spec_info;
    GX_SET_ZERO(spec_info);
    spec_info.mapEntryCount = 1;
    spec_info.pMapEntries = &spec_map_entry;
    spec_info.dataSize = sizeof(VkBool32);
    spec_info.pData = &spec_has_bones;

    std::array<VkPipelineShaderStageCreateInfo, 2> stages {};
    GX_SET_ZERO(stages);
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = pbr_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[0].pSpecializationInfo = &spec_info;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = pbr_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertexAnimated);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 6> vertex_attributes {};
    GX_SET_ZERO(vertex_attributes);
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, normal);
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 2;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = offsetof(render::PbrVertex, tangent);
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 3;
    vertex_attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[3].offset = offsetof(render::PbrVertex, uv);
    vertex_attributes[4].binding = 0;
    vertex_attributes[4].location = 4;
    vertex_attributes[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[4].offset = offsetof(render::PbrVertexAnimated, bone_weights);
    vertex_attributes[5].binding = 0;
    vertex_attributes[5].location = 5;
    vertex_attributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[5].offset = offsetof(render::PbrVertexAnimated, bone_indices);

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendAttachmentState colour_blend_attachment;
    GX_SET_ZERO(colour_blend_attachment);
    colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colour_blend;
    GX_SET_ZERO(colour_blend);
    colour_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blend.attachmentCount = 1;
    colour_blend.pAttachments = &colour_blend_attachment;

    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &colour_format;
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    VkGraphicsPipelineCreateInfo create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = &rendering_info;
    create_info.stageCount = static_cast<std::uint32_t>(stages.size());
    create_info.pStages = stages.data();
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

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_unlit_forward_pipeline(const VkFormat colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    std::array<VkPipelineShaderStageCreateInfo, 2> stages {};
    GX_SET_ZERO(stages);
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = unlit_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = unlit_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertex);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 6> vertex_attributes {};
    GX_SET_ZERO(vertex_attributes);
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, normal);
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 2;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = offsetof(render::PbrVertex, tangent);
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 3;
    vertex_attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[3].offset = offsetof(render::PbrVertex, uv);
    vertex_attributes[4].binding = 0;
    vertex_attributes[4].location = 4;
    vertex_attributes[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[4].offset = 0; // dummy bone_weights
    vertex_attributes[5].binding = 0;
    vertex_attributes[5].location = 5;
    vertex_attributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[5].offset = 0; // dummy bone_indices

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendAttachmentState colour_blend_attachment;
    GX_SET_ZERO(colour_blend_attachment);
    colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colour_blend;
    GX_SET_ZERO(colour_blend);
    colour_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blend.attachmentCount = 1;
    colour_blend.pAttachments = &colour_blend_attachment;

    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &colour_format;
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    VkGraphicsPipelineCreateInfo create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = &rendering_info;
    create_info.stageCount = static_cast<std::uint32_t>(stages.size());
    create_info.pStages = stages.data();
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

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_unlit_skinned_forward_pipeline(const VkFormat colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    constexpr VkBool32 spec_has_bones = VK_TRUE;
    VkSpecializationMapEntry spec_map_entry;
    GX_SET_ZERO(spec_map_entry);
    spec_map_entry.constantID = 0;
    spec_map_entry.size = sizeof(VkBool32);
    VkSpecializationInfo spec_info;
    GX_SET_ZERO(spec_info);
    spec_info.mapEntryCount = 1;
    spec_info.pMapEntries = &spec_map_entry;
    spec_info.dataSize = sizeof(VkBool32);
    spec_info.pData = &spec_has_bones;

    std::array<VkPipelineShaderStageCreateInfo, 2> stages {};
    GX_SET_ZERO(stages);
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = unlit_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[0].pSpecializationInfo = &spec_info;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = unlit_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertexAnimated);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 6> vertex_attributes {};
    GX_SET_ZERO(vertex_attributes);
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, normal);
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 2;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = offsetof(render::PbrVertex, tangent);
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 3;
    vertex_attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[3].offset = offsetof(render::PbrVertex, uv);
    vertex_attributes[4].binding = 0;
    vertex_attributes[4].location = 4;
    vertex_attributes[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[4].offset = offsetof(render::PbrVertexAnimated, bone_weights);
    vertex_attributes[5].binding = 0;
    vertex_attributes[5].location = 5;
    vertex_attributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[5].offset = offsetof(render::PbrVertexAnimated, bone_indices);

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendAttachmentState colour_blend_attachment;
    GX_SET_ZERO(colour_blend_attachment);
    colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colour_blend;
    GX_SET_ZERO(colour_blend);
    colour_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blend.attachmentCount = 1;
    colour_blend.pAttachments = &colour_blend_attachment;

    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &colour_format;
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    VkGraphicsPipelineCreateInfo create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = &rendering_info;
    create_info.stageCount = static_cast<std::uint32_t>(stages.size());
    create_info.pStages = stages.data();
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

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_skybox_equirectangular_pipeline(const VkFormat colour_format)
{
    const auto& bindless = descriptor::Bindless::get();

    std::array<VkPipelineShaderStageCreateInfo, 2> stages {};
    GX_SET_ZERO(stages);
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = skybox_equirectangular_vert_sm->get_vulkan_data();
    stages[0].pName = default_stage_entry;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = skybox_equirectangular_frag_sm->get_vulkan_data();
    stages[1].pName = default_stage_entry;

    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertex);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Skybox only uses locations 0-3 (position, normal, tangent, uv), no bone attributes
    std::array<VkVertexInputAttributeDescription, 4> vertex_attributes {};
    GX_SET_ZERO(vertex_attributes);
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, normal);
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 2;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = offsetof(render::PbrVertex, tangent);
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 3;
    vertex_attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[3].offset = offsetof(render::PbrVertex, uv);

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_FALSE; // skybox does not write depth
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendAttachmentState colour_blend_attachment;
    GX_SET_ZERO(colour_blend_attachment);
    colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colour_blend;
    GX_SET_ZERO(colour_blend);
    colour_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blend.attachmentCount = 1;
    colour_blend.pAttachments = &colour_blend_attachment;

    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &colour_format;
    rendering_info.depthAttachmentFormat = fixed_depth_format;

    VkGraphicsPipelineCreateInfo create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = &rendering_info;
    create_info.stageCount = static_cast<std::uint32_t>(stages.size());
    create_info.pStages = stages.data();
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

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_shadow_pipeline(const VkFormat depth_format)
{
    const auto& bindless = descriptor::Bindless::get();

    VkPipelineShaderStageCreateInfo vert_stage;
    GX_SET_ZERO(vert_stage);
    vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage.module = shadow_caster_vert_sm->get_vulkan_data();
    vert_stage.pName = default_stage_entry;

    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertex);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Shadow caster uses position(0), uv(3), dummy bone_weights(4), dummy bone_indices(5)
    std::array<VkVertexInputAttributeDescription, 4> vertex_attributes {};
    GX_SET_ZERO(vertex_attributes);
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 3;
    vertex_attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, uv);
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 4;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = 0; // dummy bone_weights
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 5;
    vertex_attributes[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[3].offset = 0; // dummy bone_indices

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendStateCreateInfo colour_blend;
    GX_SET_ZERO(colour_blend);
    colour_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blend.attachmentCount = 0;
    colour_blend.pAttachments = nullptr;

    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 0;
    rendering_info.pColorAttachmentFormats = nullptr;
    rendering_info.depthAttachmentFormat = depth_format;

    VkGraphicsPipelineCreateInfo create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = &rendering_info;
    create_info.stageCount = 1;
    create_info.pStages = &vert_stage;
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

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_skinned_shadow_pipeline(const VkFormat depth_format)
{
    const auto& bindless = descriptor::Bindless::get();

    constexpr VkBool32 spec_has_bones = VK_TRUE;
    VkSpecializationMapEntry spec_map_entry;
    GX_SET_ZERO(spec_map_entry);
    spec_map_entry.constantID = 0;
    spec_map_entry.size = sizeof(VkBool32);
    VkSpecializationInfo spec_info;
    GX_SET_ZERO(spec_info);
    spec_info.mapEntryCount = 1;
    spec_info.pMapEntries = &spec_map_entry;
    spec_info.dataSize = sizeof(VkBool32);
    spec_info.pData = &spec_has_bones;

    VkPipelineShaderStageCreateInfo vert_stage;
    GX_SET_ZERO(vert_stage);
    vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage.module = shadow_caster_vert_sm->get_vulkan_data();
    vert_stage.pName = default_stage_entry;
    vert_stage.pSpecializationInfo = &spec_info;

    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertexAnimated);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 4> vertex_attributes {};
    GX_SET_ZERO(vertex_attributes);
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 3;
    vertex_attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, uv);
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 4;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = offsetof(render::PbrVertexAnimated, bone_weights);
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 5;
    vertex_attributes[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[3].offset = offsetof(render::PbrVertexAnimated, bone_indices);

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendStateCreateInfo colour_blend;
    GX_SET_ZERO(colour_blend);
    colour_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blend.attachmentCount = 0;
    colour_blend.pAttachments = nullptr;

    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 0;
    rendering_info.pColorAttachmentFormats = nullptr;
    rendering_info.depthAttachmentFormat = depth_format;

    VkGraphicsPipelineCreateInfo create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = &rendering_info;
    create_info.stageCount = 1;
    create_info.pStages = &vert_stage;
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

gearoenix::vulkan::pipeline::FormatPipelines gearoenix::vulkan::pipeline::Manager::create_format_pipelines(const VkFormat colour_format)
{
    GX_ASSERT_D(colour_format != VK_FORMAT_UNDEFINED);
    FormatPipelines fp;
    fp.pbr_forward = create_pbr_forward_pipeline(colour_format);
    fp.pbr_skinned_forward = create_pbr_skinned_forward_pipeline(colour_format);
    fp.unlit_forward = create_unlit_forward_pipeline(colour_format);
    fp.unlit_skinned_forward = create_unlit_skinned_forward_pipeline(colour_format);
    fp.skybox_equirectangular = create_skybox_equirectangular_pipeline(colour_format);
    return fp;
}

const gearoenix::vulkan::pipeline::FormatPipelines& gearoenix::vulkan::pipeline::Manager::get_pipelines(const VkFormat colour_format)
{
    auto& fp = formats_pipelines[colour_format];
    if (nullptr == fp.pbr_forward) {
        fp = create_format_pipelines(colour_format);
    }
    return fp;
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_ray_tracing_pbr(const std::shared_ptr<descriptor::SetLayout>&)
{
    GX_UNIMPLEMENTED;
}

#endif
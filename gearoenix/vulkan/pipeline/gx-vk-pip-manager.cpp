#include "gx-vk-pip-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../render/gx-rnd-vertex.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../shader/gx-vk-shd-manager.hpp"
#include "../shader/gx-vk-shd-module.hpp"
#include "gx-vk-pip-cache.hpp"
#include "gx-vk-pip-pipeline.hpp"

namespace {
constexpr char default_stage_entry[] = "main";
}

// TODO: Separate ray tracing and rasterisation pipeline initialisation into distinct struct.
// TODO: Use specialisation constants to split monolithic shaders (e.g. unlit) into dedicated pipeline
//       variants per feature (alpha blending, UV transform, albedo source) instead of a single
//       uber-shader that handles all combinations at runtime.
// TODO: Pipeline creation must be based on demand, instead of creating all of them.

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

void gearoenix::vulkan::pipeline::Manager::initialise_rasterizer()
{
    pbr_vert_sm = shader_manager->get("forward-pbr.vert");
    pbr_frag_sm = shader_manager->get("forward-pbr.frag");

    const auto& physical_device = device::Physical::get();
    const auto& bindless = descriptor::Bindless::get();
    const auto depth_format = physical_device.get_supported_depth_format();
    const auto color_format = Swapchain::get().get_format().format;

    std::array<VkPipelineShaderStageCreateInfo, 2> stages{};
    GX_SET_ZERO(stages);

    // ========== Shader stages ==========
    auto& vert_stage = stages[0];
    vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage.module = pbr_vert_sm->get_vulkan_data();
    vert_stage.pName = default_stage_entry;

    auto& frag_stage = stages[1];
    frag_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage.module = pbr_frag_sm->get_vulkan_data();
    frag_stage.pName = default_stage_entry;

    // ========== Vertex input ==========
    VkVertexInputBindingDescription vertex_binding;
    GX_SET_ZERO(vertex_binding);
    vertex_binding.binding = 0;
    vertex_binding.stride = sizeof(render::PbrVertex);
    vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Non-skinned pipelines need 6 attributes: locations 0-3 for actual data,
    // and dummy locations 4-5 for bone_weights/bone_indices that shaders declare
    // but won't use when GX_SPEC_HAS_BONES is false.
    std::array<VkVertexInputAttributeDescription, 6> vertex_attributes{};
    GX_SET_ZERO(vertex_attributes);

    // position: vec3
    vertex_attributes[0].binding = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[0].offset = offsetof(render::PbrVertex, position);

    // normal: vec3
    vertex_attributes[1].binding = 0;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attributes[1].offset = offsetof(render::PbrVertex, normal);

    // tangent: vec4
    vertex_attributes[2].binding = 0;
    vertex_attributes[2].location = 2;
    vertex_attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[2].offset = offsetof(render::PbrVertex, tangent);

    // uv: vec2
    vertex_attributes[3].binding = 0;
    vertex_attributes[3].location = 3;
    vertex_attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attributes[3].offset = offsetof(render::PbrVertex, uv);

    // bone_weights: vec4 (dummy, shader won't use when GX_SPEC_HAS_BONES is false)
    vertex_attributes[4].binding = 0;
    vertex_attributes[4].location = 4;
    vertex_attributes[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[4].offset = 0;

    // bone_indices: vec4 (dummy, shader won't use when GX_SPEC_HAS_BONES is false)
    vertex_attributes[5].binding = 0;
    vertex_attributes[5].location = 5;
    vertex_attributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attributes[5].offset = 0;

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    GX_SET_ZERO(vertex_input_info);
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();

    // ========== Input assembly ==========
    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    GX_SET_ZERO(input_assembly);
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // ========== Dynamic state (viewport & scissor) ==========
    constexpr std::array dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamic_state;
    GX_SET_ZERO(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    // ========== Viewport state (dynamic, so counts only) ==========
    VkPipelineViewportStateCreateInfo viewport_state;
    GX_SET_ZERO(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    // ========== Rasterization ==========
    VkPipelineRasterizationStateCreateInfo rasterization;
    GX_SET_ZERO(rasterization);
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    // ========== Multisampling ==========
    VkPipelineMultisampleStateCreateInfo multisample;
    GX_SET_ZERO(multisample);
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // ========== Depth/stencil ==========
    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    GX_SET_ZERO(depth_stencil);
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    // ========== Color blending (forward) ==========
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    GX_SET_ZERO(color_blend_attachment);
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo color_blend;
    GX_SET_ZERO(color_blend);
    color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &color_blend_attachment;

    // ========== Dynamic rendering (forward) ==========
    VkPipelineRenderingCreateInfo rendering_info;
    GX_SET_ZERO(rendering_info);
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &color_format;
    rendering_info.depthAttachmentFormat = depth_format;

    // ========== Forward pipeline ==========
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
    create_info.pColorBlendState = &color_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = bindless.get_pipeline_layout();

    pbr_forward_pipeline = Pipeline::construct_graphics(std::shared_ptr(cache), create_info);

    // ========== Shadow pipeline (depth-only, vertex shader only) ==========
    // Reuse rendering_info and color_blend, modifying only what differs
    rendering_info.colorAttachmentCount = 0;
    rendering_info.pColorAttachmentFormats = nullptr;

    color_blend.attachmentCount = 0;
    color_blend.pAttachments = nullptr;

    create_info.stageCount = 1;

    pbr_shadow_pipeline = Pipeline::construct_graphics(std::shared_ptr(cache), create_info);

    // ========== Unlit Forward Pipeline (non-skinned) ==========
    unlit_vert_sm = shader_manager->get("forward-unlit.vert");
    unlit_frag_sm = shader_manager->get("forward-unlit.frag");

    // Reuse vert_stage and frag_stage, only updating module
    vert_stage.module = unlit_vert_sm->get_vulkan_data();
    frag_stage.module = unlit_frag_sm->get_vulkan_data();

    // Restore forward pipeline settings
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &color_format;

    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &color_blend_attachment;

    create_info.stageCount = static_cast<std::uint32_t>(stages.size());

    unlit_forward_pipeline = Pipeline::construct_graphics(std::shared_ptr(cache), create_info);

    // ========== Skinned pipelines ==========
    // Update vertex_attributes for skinned: set proper bone offsets (locations 4-5)
    vertex_attributes[4].offset = offsetof(render::PbrVertexAnimated, bone_weights);
    vertex_attributes[5].offset = offsetof(render::PbrVertexAnimated, bone_indices);

    // Update binding stride for skinned vertex data
    vertex_binding.stride = sizeof(render::PbrVertexAnimated);

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

    // Skinned PBR forward pipeline - reuse vert_stage with PBR shaders
    vert_stage.module = pbr_vert_sm->get_vulkan_data();
    frag_stage.module = pbr_frag_sm->get_vulkan_data();
    vert_stage.pSpecializationInfo = &spec_info;

    create_info.stageCount = static_cast<std::uint32_t>(stages.size());

    pbr_skinned_forward_pipeline = Pipeline::construct_graphics(std::shared_ptr(cache), create_info);

    // Skinned PBR shadow pipeline
    rendering_info.colorAttachmentCount = 0;
    rendering_info.pColorAttachmentFormats = nullptr;

    color_blend.attachmentCount = 0;
    color_blend.pAttachments = nullptr;

    create_info.stageCount = 1;

    pbr_skinned_shadow_pipeline = Pipeline::construct_graphics(std::shared_ptr(cache), create_info);

    // Skinned unlit forward pipeline - reuse vert_stage with unlit shaders
    vert_stage.module = unlit_vert_sm->get_vulkan_data();
    frag_stage.module = unlit_frag_sm->get_vulkan_data();

    // Restore forward pipeline settings
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &color_format;

    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &color_blend_attachment;

    create_info.stageCount = static_cast<std::uint32_t>(stages.size());

    unlit_skinned_forward_pipeline = Pipeline::construct_graphics(std::shared_ptr(cache), create_info);
}

gearoenix::vulkan::pipeline::Manager::Manager()
    : Singleton(this)
    , cache(new Cache())
    , shader_manager(new shader::Manager())
{
    if (device::Physical::get().get_rtx_supported()) {
        initialise_ray_tracing();
    } else {
        initialise_rasterizer();
    }
}

gearoenix::vulkan::pipeline::Manager::~Manager() = default;

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_ray_tracing_pbr(
    const std::shared_ptr<descriptor::SetLayout>&)
{
    GX_UNIMPLEMENTED;
}

#endif

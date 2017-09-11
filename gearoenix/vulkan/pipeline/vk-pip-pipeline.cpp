#include "vk-pip-pipeline.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/cr-static.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../shader/vk-shader-diffuse-colored.hpp"
#include "../shader/vk-shader-manager.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "../vk-render-pass.hpp"
#include "../vk-surface.hpp"
#include "../vk-swapchain.hpp"
#include "vk-pip-cache.hpp"
#include "vk-pip-layout.hpp"
#include <fstream>

gearoenix::render::pipeline::Pipeline::Pipeline(
    const std::shared_ptr<Cache>& cache, const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<RenderPass>& render_pass,
    std::shared_ptr<shader::Manager>& shader_manager)
    : cache(cache)
    , layout(layout)
    , render_pass(render_pass)
{
    auto& d = cache->get_logical_device();
    auto& l = d->get_physical_device()->get_instance()->get_linker();
    VkGraphicsPipelineCreateInfo pipeline_create_info;
    setz(pipeline_create_info);
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.layout = layout->get_vulkan_data();
    pipeline_create_info.renderPass = render_pass->get_vulkan_data();
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
    setz(input_assembly_state);
    input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineRasterizationStateCreateInfo rasterization_state;
    setz(rasterization_state);
    rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state.cullMode = VK_CULL_MODE_NONE;
    rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state.lineWidth = 1.0f;
    VkPipelineColorBlendAttachmentState blend_attachment_state;
    setz(blend_attachment_state);
    blend_attachment_state.colorWriteMask = 0xF;
    VkPipelineColorBlendStateCreateInfo color_blend_state;
    setz(color_blend_state);
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &blend_attachment_state;
    VkPipelineViewportStateCreateInfo viewport_state;
    setz(viewport_state);
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    const VkDynamicState dynamic_state_enables[] = { VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic_state;
    setz(dynamic_state);
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.pDynamicStates = dynamic_state_enables;
    dynamic_state.dynamicStateCount = countof(dynamic_state_enables);
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
    setz(depth_stencil_state);
    depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.depthTestEnable = 1;
    depth_stencil_state.depthWriteEnable = 1;
    depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil_state.depthBoundsTestEnable = 0;
    depth_stencil_state.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil_state.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil_state.stencilTestEnable = 0;
    depth_stencil_state.front = depth_stencil_state.back;
    VkPipelineMultisampleStateCreateInfo multisample_state;
    setz(multisample_state);
    multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkVertexInputBindingDescription vertex_input_binding;
    setz(vertex_input_binding);
    vertex_input_binding.stride = 24;
    vertex_input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    VkVertexInputAttributeDescription vertex_attribute[2];
    setz(vertex_attribute);
    vertex_attribute[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attribute[1].location = 1;
    vertex_attribute[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attribute[1].offset = 12;
    VkPipelineVertexInputStateCreateInfo vertex_input_state;
    setz(vertex_input_state);
    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.vertexBindingDescriptionCount = 1;
    vertex_input_state.pVertexBindingDescriptions = &vertex_input_binding;
    vertex_input_state.vertexAttributeDescriptionCount = 2;
    vertex_input_state.pVertexAttributeDescriptions = vertex_attribute;
    const char* stage_name = "main";
    diffuse = std::static_pointer_cast<shader::DiffuseColored>(
        shader_manager->get_shader(d, 1));
    VkPipelineShaderStageCreateInfo shader_stages[DIFFUSE_COLORED_MODULE_COUNT];
    setz(shader_stages);
    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = diffuse->get_module(0);
    shader_stages[0].pName = stage_name;
    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = diffuse->get_module(1);
    shader_stages[1].pName = stage_name;
    pipeline_create_info.stageCount = countof(shader_stages);
    pipeline_create_info.pStages = shader_stages;
    pipeline_create_info.pVertexInputState = &vertex_input_state;
    pipeline_create_info.pInputAssemblyState = &input_assembly_state;
    pipeline_create_info.pRasterizationState = &rasterization_state;
    pipeline_create_info.pColorBlendState = &color_blend_state;
    pipeline_create_info.pMultisampleState = &multisample_state;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pDepthStencilState = &depth_stencil_state;
    pipeline_create_info.renderPass = render_pass->get_vulkan_data();
    pipeline_create_info.pDynamicState = &dynamic_state;
    VKC(l->vkCreateGraphicsPipelines(
        d->get_vulkan_data(), cache->get_vulkan_data(), 1, &pipeline_create_info,
        nullptr, &vulkan_data));
}

gearoenix::render::pipeline::Pipeline::~Pipeline()
{
    auto& d = cache->get_logical_device();
    auto& l = d->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyPipeline(d->get_vulkan_data(), vulkan_data, nullptr);
}

const VkPipeline& gearoenix::render::pipeline::Pipeline::get_vulkan_data()
    const
{
    return vulkan_data;
}

#include "vk-pip-pipeline.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/cr-static.hpp"
#include "../../render/shader/rnd-shd-shader.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "../vk-render-pass.hpp"
#include "../vk-surface.hpp"
#include "../vk-swapchain.hpp"
#include "vk-pip-cache.hpp"
#include "vk-pip-layout.hpp"

gearoenix::render::pipeline::Pipeline::Pipeline(shader::Id sid,
    Cache* cache,
    RenderPass* rndpass,
    const std::shared_ptr<shader::Shader>& shd,
    const std::shared_ptr<descriptor::Set>& desset)
    : sid(sid)
    , dev(cache->get_logical_device())
    , cache(cache)
    , layout(new Layout(desset))
    , rndpass(rndpass)
    , shd(shd)
    , desset(desset)
{
    const VkDevice vkdev = dev->get_vulkan_data();
    const device::Physical* p = dev->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkGraphicsPipelineCreateInfo pipeline_create_info;
    setz(pipeline_create_info);
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.layout = layout->get_vulkan_data();
    pipeline_create_info.renderPass = rndpass->get_vulkan_data();
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
    setz(input_assembly_state);
    input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineRasterizationStateCreateInfo rasterization_state;
    setz(rasterization_state);
    rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
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
    const VkDynamicState dynamic_state_enables[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
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
    const std::vector<shader::VertexAttribute> shdvtxatt = shader::Shader::get_vertex_atributes(sid);
    std::vector<VkVertexInputAttributeDescription> vertex_attributes(shdvtxatt.size());
    const uint32_t vtxattcnt = shdvtxatt.size();
    uint32_t vtxattoff = 0;
    for (uint32_t i = 0; i < vtxattcnt; ++i) {
        vertex_attributes[i].binding = 0; // maybe change in future
        vertex_attributes[i].location = i;
        vertex_attributes[i].offset = vtxattoff;
        switch (shdvtxatt[i]) {
        case shader::VEC3F32:
            vertex_attributes[i].format = VK_FORMAT_R32G32B32_SFLOAT;
            vtxattoff += 12;
            break;
        case shader::VEC2F32:
            vertex_attributes[i].format = VK_FORMAT_R32G32_SFLOAT;
            vtxattoff += 8;
            break;
        default:
            LOGF("Unexpected");
        }
    }
    VkVertexInputBindingDescription vertex_input_binding;
    setz(vertex_input_binding);
    vertex_input_binding.stride = vtxattoff;
    vertex_input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    VkPipelineVertexInputStateCreateInfo vertex_input_state;
    setz(vertex_input_state);
    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.vertexBindingDescriptionCount = 1;
    vertex_input_state.pVertexBindingDescriptions = &vertex_input_binding;
    vertex_input_state.vertexAttributeDescriptionCount = vertex_attributes.size();
    vertex_input_state.pVertexAttributeDescriptions = vertex_attributes.data();
    const char* stage_name = "main";
    const std::vector<shader::stage::Id>& stages_ids = shd->get_stages_ids();
    uint32_t stagescnt = stages_ids.size();
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages(stagescnt);
    for (uint32_t i = 0; i < stagescnt; ++i) {
        setz(shader_stages[i]);
        shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        switch (stages_ids[i]) {
        case shader::stage::VERTEX:
            shader_stages[i].stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case shader::stage::FRAGMENT:
            shader_stages[i].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            LOGF("Unexpected");
        }
        shader_stages[i].module = shd->get_stage(stages_ids[i])->get_vulkan_data();
        shader_stages[i].pName = stage_name;
    }
    pipeline_create_info.stageCount = shader_stages.size();
    pipeline_create_info.pStages = shader_stages.data();
    pipeline_create_info.pVertexInputState = &vertex_input_state;
    pipeline_create_info.pInputAssemblyState = &input_assembly_state;
    pipeline_create_info.pRasterizationState = &rasterization_state;
    pipeline_create_info.pColorBlendState = &color_blend_state;
    pipeline_create_info.pMultisampleState = &multisample_state;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pDepthStencilState = &depth_stencil_state;
    pipeline_create_info.renderPass = rndpass->get_vulkan_data();
    pipeline_create_info.pDynamicState = &dynamic_state;
    VKC(l->vkCreateGraphicsPipelines(
        vkdev, cache->get_vulkan_data(), 1, &pipeline_create_info,
        nullptr, &vulkan_data));
}

gearoenix::render::pipeline::Pipeline::~Pipeline()
{
    const device::Physical* p = dev->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    l->vkDestroyPipeline(dev->get_vulkan_data(), vulkan_data, nullptr);
    delete layout;
}

const VkPipeline& gearoenix::render::pipeline::Pipeline::get_vulkan_data()
    const
{
    return vulkan_data;
}

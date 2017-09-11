#include "vk-render-pass.hpp"
#include "../core/cr-static.hpp"
#include "device/vk-dev-logical.hpp"
#include "device/vk-dev-physical.hpp"
#include "vk-check.hpp"
#include "vk-instance.hpp"
#include "vk-swapchain.hpp"

gearoenix::render::RenderPass::RenderPass(
    const std::shared_ptr<Swapchain>& swapchain)
    : swapchain(swapchain)
{
    auto& d = swapchain->get_logical_device();
    auto& p = d->get_physical_device();
    auto& l = p->get_instance()->get_linker();
    VkAttachmentDescription attachment_descriptions[2];
    setz(attachment_descriptions);
    attachment_descriptions[0].format = swapchain->get_chosen_format().format;
    attachment_descriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_descriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_descriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_descriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachment_descriptions[1].format = p->get_supported_depth_format();
    attachment_descriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_descriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_descriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_descriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_descriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_descriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_descriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkAttachmentReference colour_reference;
    setz(colour_reference);
    colour_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colour_reference.attachment = 0;
    VkAttachmentReference depth_reference;
    setz(depth_reference);
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_reference.attachment = 1;
    VkSubpassDescription subpass_description;
    setz(subpass_description);
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &colour_reference;
    subpass_description.pDepthStencilAttachment = &depth_reference;
    VkSubpassDependency dependencies[2];
    setz(dependencies);
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    VkRenderPassCreateInfo render_pass_create_info;
    setz(render_pass_create_info);
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = countof(attachment_descriptions);
    render_pass_create_info.pAttachments = attachment_descriptions;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = countof(dependencies);
    render_pass_create_info.pDependencies = dependencies;
    VKC(l->vkCreateRenderPass(d->get_vulkan_data(), &render_pass_create_info, 0,
        &vulkan_data));
}

gearoenix::render::RenderPass::~RenderPass()
{
    auto& d = swapchain->get_logical_device();
    auto& p = d->get_physical_device();
    auto& l = p->get_instance()->get_linker();
    l->vkDestroyRenderPass(d->get_vulkan_data(), vulkan_data, nullptr);
}

const VkRenderPass& gearoenix::render::RenderPass::get_vulkan_data() const
{
    return vulkan_data;
}

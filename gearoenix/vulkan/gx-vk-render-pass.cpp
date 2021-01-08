#include "gx-vk-render-pass.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-swapchain.hpp"

gearoenix::vulkan::RenderPass::RenderPass(std::shared_ptr<Swapchain> sw) noexcept
    : swapchain(std::move(sw))
{
    const auto& d = swapchain->get_logical_device();
    const auto& p = d->get_physical_device();

    VkAttachmentDescription attachment_descriptions[2];
    GX_SET_ZERO(attachment_descriptions)
    attachment_descriptions[0].format = swapchain->get_format().format;
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
    GX_SET_ZERO(colour_reference)
    colour_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colour_reference.attachment = 0;

    VkAttachmentReference depth_reference;
    GX_SET_ZERO(depth_reference)
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_reference.attachment = 1;

    VkSubpassDescription subpass_description;
    GX_SET_ZERO(subpass_description)
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &colour_reference;
    subpass_description.pDepthStencilAttachment = &depth_reference;

    VkSubpassDependency dependencies[2];
    GX_SET_ZERO(dependencies)
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
    GX_SET_ZERO(render_pass_create_info)
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = GX_COUNT_OF(attachment_descriptions);
    render_pass_create_info.pAttachments = attachment_descriptions;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = GX_COUNT_OF(dependencies);
    render_pass_create_info.pDependencies = dependencies;

    GX_VK_CHK_L(vkCreateRenderPass(d->get_vulkan_data(), &render_pass_create_info, nullptr, &vulkan_data))
}

gearoenix::vulkan::RenderPass::~RenderPass() noexcept
{
    Loader::vkDestroyRenderPass(swapchain->get_logical_device()->get_vulkan_data(), vulkan_data, nullptr);
}

#endif

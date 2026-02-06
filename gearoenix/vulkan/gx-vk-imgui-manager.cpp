#include "gx-vk-imgui-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "command/gx-vk-cmd-buffer.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "engine/gx-vk-eng-engine.hpp"
#include "engine/gx-vk-eng-frame.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-instance.hpp"
#include "gx-vk-swapchain.hpp"
#include "image/gx-vk-img-image.hpp"
#include "image/gx-vk-img-view.hpp"
#include "pipeline/gx-vk-pip-cache.hpp"
#include "pipeline/gx-vk-pip-manager.hpp"
#include "queue/gx-vk-qu-queue.hpp"

#include <ImGui/backends/imgui_impl_vulkan.h>

gearoenix::vulkan::ImGuiManager::ImGuiManager()
{
    const auto vk_dev = device::Logical::get().get_vulkan_data();

    // Create a dedicated descriptor pool for ImGui
    constexpr std::uint32_t pool_size = 100;
    constexpr std::array pool_sizes {
        VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, pool_size },
    };

    VkDescriptorPoolCreateInfo pool_info;
    GX_SET_ZERO(pool_info);
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = pool_size;
    pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();

    GX_VK_CHK(vkCreateDescriptorPool(vk_dev, &pool_info, nullptr, &descriptor_pool));

    const auto api_version = Instance::get().get_api_version();

    // Initialize ImGui Vulkan backend with dynamic rendering
    ImGui_ImplVulkan_InitInfo info {};
    info.ApiVersion = api_version;
    info.Instance = Instance::get().get_vulkan_data();
    info.PhysicalDevice = device::Physical::get().get_vulkan_data();
    info.Device = vk_dev;
    info.QueueFamily = device::Physical::get().get_graphics_queue_node_index();
    info.Queue = queue::Queue::get().get_vulkan_data();
    info.DescriptorPool = descriptor_pool;
    info.MinImageCount = static_cast<decltype(info.MinImageCount)>(Swapchain::get().get_image_views().size());
    info.ImageCount = info.MinImageCount;
    info.PipelineCache = pipeline::Manager::get().get_cache()->get_vulkan_data();
    info.UseDynamicRendering = true;
    info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &Swapchain::get().get_format().format;
#if GX_DEBUG_MODE
    info.CheckVkResultFn = +[](const VkResult result) {
        GX_VK_CHK(result);
    };
#endif

    ImGui_ImplVulkan_LoadFunctions(api_version, +[](const char* const name, void*) {
        return Loader::get(name);
    }, nullptr);
    ImGui_ImplVulkan_Init(&info);
}

gearoenix::vulkan::ImGuiManager::~ImGuiManager()
{
    device::Logical::get().wait_to_finish();
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(device::Logical::get().get_vulkan_data(), descriptor_pool, nullptr);
}

void gearoenix::vulkan::ImGuiManager::upload_fonts()
{
    // Font textures are handled automatically by modern ImGui Vulkan backend
    (void)this;
}

void gearoenix::vulkan::ImGuiManager::start_frame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
}

void gearoenix::vulkan::ImGuiManager::end_frame()
{
    ImGui::Render();
}

void gearoenix::vulkan::ImGuiManager::update()
{
    auto& e = core::Singleton<engine::Engine>::get();
    const auto& frame = e.get_current_frame();
    const auto& swapchain_view = *frame.view;
    auto& swapchain_image = *swapchain_view.get_image();
    const auto vk_cmd = frame.cmd->get_vulkan_data();
    const auto vk_image = swapchain_image.get_vulkan_data();

    // Capture original layout before any transitions to determine if scene content is present
    const auto original_layout = swapchain_image.get_layout();
    const bool scene_content_present = original_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    auto* const draw_data = ImGui::GetDrawData();
    const bool has_draw_data = nullptr != draw_data && draw_data->TotalVtxCount > 0;

    if (has_draw_data) {
        // Transition swapchain image to color attachment optimal if needed
        if (original_layout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            const bool from_undefined = original_layout == VK_IMAGE_LAYOUT_UNDEFINED;
            VkImageMemoryBarrier barrier;
            GX_SET_ZERO(barrier);
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.srcAccessMask = from_undefined ? 0 : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout = original_layout;
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = vk_image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(
                vk_cmd,
                from_undefined ? VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0, 0, nullptr, 0, nullptr, 1, &barrier);

            swapchain_image.set_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }

        // Begin dynamic rendering
        VkRenderingAttachmentInfo color_attachment;
        GX_SET_ZERO(color_attachment);
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = swapchain_view.get_vulkan_data();
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        // If scene was blitted (came from COLOR_ATTACHMENT_OPTIMAL), use LOAD to preserve the content
        color_attachment.loadOp = scene_content_present ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

        VkRenderingInfo rendering_info;
        GX_SET_ZERO(rendering_info);
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea.extent = { swapchain_view.get_extent().width, swapchain_view.get_extent().height };
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment;

        vkCmdBeginRendering(vk_cmd, &rendering_info);

        ImGui_ImplVulkan_RenderDrawData(draw_data, vk_cmd);

        vkCmdEndRendering(vk_cmd);

        // Transition swapchain image to present
        {
            VkImageMemoryBarrier barrier;
            GX_SET_ZERO(barrier);
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = 0;
            barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = vk_image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(
                vk_cmd,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0, 0, nullptr, 0, nullptr, 1, &barrier);
        }
    } else {
        // No ImGui draw data - just transition to PRESENT based on current layout
        VkImageMemoryBarrier barrier;
        GX_SET_ZERO(barrier);
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask = scene_content_present ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT : 0;
        barrier.dstAccessMask = 0;
        barrier.oldLayout = original_layout;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = vk_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            vk_cmd,
            scene_content_present ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    swapchain_image.set_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

#endif
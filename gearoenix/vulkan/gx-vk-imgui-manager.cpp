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

    // Determine the colour format for ImGui rendering.
    // If the swapchain has a UNORM imgui_view (mutable sRGB swapchain), use the UNORM format
    // to avoid double gamma correction. Otherwise, use the swapchain format as-is.
    const auto& swapchain = Swapchain::get();
    const auto& swapchain_format = swapchain.get_format().format;
    if (swapchain.get_frames()[0].imgui_view) {
        switch (swapchain_format) {
        case VK_FORMAT_R8G8B8A8_SRGB:
            imgui_colour_format = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case VK_FORMAT_B8G8R8A8_SRGB:
            imgui_colour_format = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        default:
            imgui_colour_format = swapchain_format;
            break;
        }
    } else {
        imgui_colour_format = swapchain_format;
    }

    // Initialize ImGui Vulkan backend with dynamic rendering
    ImGui_ImplVulkan_InitInfo info {};
    info.ApiVersion = api_version;
    info.Instance = Instance::get().get_vulkan_data();
    info.PhysicalDevice = device::Physical::get().get_vulkan_data();
    info.Device = vk_dev;
    info.QueueFamily = device::Physical::get().get_graphics_queue_node_index();
    info.Queue = queue::Queue::get().get_vulkan_data();
    info.DescriptorPool = descriptor_pool;
    info.MinImageCount = static_cast<decltype(info.MinImageCount)>(frames_in_flight);
    info.ImageCount = info.MinImageCount;
    info.PipelineCache = pipeline::Manager::get().get_cache()->get_vulkan_data();
    info.UseDynamicRendering = true;
    info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &imgui_colour_format;
#if GX_DEBUG_MODE
    info.CheckVkResultFn = +[](const VkResult result) {
        GX_VK_CHK(result);
    };
#endif

    ImGui_ImplVulkan_LoadFunctions(api_version, +[](const char* const name, void*) { return Loader::get(name); }, nullptr);
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
    // Use the UNORM view for ImGui rendering if available, to avoid double gamma correction
    const auto& imgui_render_view = frame.imgui_view ? *frame.imgui_view : swapchain_view;
    auto& swapchain_image = *swapchain_view.get_image();
    const auto vk_cmd = frame.cmd->get_vulkan_data();

    // Capture if scene content is present before any transitions to determine the load operation
    const bool scene_content_present = swapchain_image.get_layout() == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    if (auto* const draw_data = ImGui::GetDrawData(); nullptr != draw_data && draw_data->TotalVtxCount > 0) {
        // Transition swapchain image to colour attachment optimal if needed
        swapchain_image.transit(vk_cmd, image::TransitionRequest::color_attachment());

        // Begin dynamic rendering
        VkRenderingAttachmentInfo color_attachment;
        GX_SET_ZERO(color_attachment);
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = imgui_render_view.get_vulkan_data();
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        // If a scene was blitted (came from COLOR_ATTACHMENT_OPTIMAL), use LOAD to preserve the content.
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
    }

    // Transition swapchain image to present
    swapchain_image.transit(vk_cmd, image::TransitionRequest::present());
}

#endif
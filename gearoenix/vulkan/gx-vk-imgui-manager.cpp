#include "gx-vk-imgui-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
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
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, pool_size },
    };

    const vk::DescriptorPoolCreateInfo pool_info {
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, pool_size, pool_sizes
    };

    descriptor_pool = vk::raii::DescriptorPool(device::Logical::get().get_device(), pool_info);

    const auto api_version = Instance::get().get_api_version();

    // ImGui renders directly into the swapchain view; the swapchain is now UNORM-only
    // (no mutable-format aliasing), so there's a single format to use for both the
    // pipeline and the runtime attachment.
    const auto& swapchain_colour_format = Swapchain::get().get_format().format;

    // Initialize ImGui Vulkan backend with dynamic rendering
    ImGui_ImplVulkan_InitInfo info { };
    info.ApiVersion = api_version;
    info.Instance = static_cast<VkInstance>(Instance::get().get_vulkan_data());
    info.PhysicalDevice = static_cast<VkPhysicalDevice>(device::Physical::get().get_vulkan_data());
    info.Device = static_cast<VkDevice>(vk_dev);
    info.QueueFamily = device::Physical::get().get_graphics_queue_node_index();
    info.Queue = static_cast<VkQueue>(queue::Queue::get().get_vulkan_data());
    info.DescriptorPool = static_cast<VkDescriptorPool>(*descriptor_pool);
    info.MinImageCount = static_cast<decltype(info.MinImageCount)>(frames_in_flight);
    info.ImageCount = info.MinImageCount;
    info.PipelineCache = static_cast<VkPipelineCache>(pipeline::Manager::get().get_cache()->get_vulkan_data());
    info.UseDynamicRendering = true;
    info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    info.PipelineRenderingCreateInfo.pColorAttachmentFormats = reinterpret_cast<const VkFormat*>(&swapchain_colour_format);
#if GX_DEBUG_MODE
    info.CheckVkResultFn = +[](const VkResult result) { GX_VK_CHK_L(result); };
#endif

    ImGui_ImplVulkan_LoadFunctions(
        api_version,
        +[](const char* const name, void* user_data) {
            return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(static_cast<VkInstance>(user_data), name);
        },
        static_cast<VkInstance>(Instance::get().get_vulkan_data()));
    ImGui_ImplVulkan_Init(&info);
}

gearoenix::vulkan::ImGuiManager::~ImGuiManager()
{
    device::Logical::get().wait_to_finish();
    ImGui_ImplVulkan_Shutdown();
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
    const auto& swapchain_frame = Swapchain::get().get_frame();
    const auto& swapchain_view = *swapchain_frame.view;
    auto& swapchain_image = *swapchain_view.get_image();
    const auto vk_cmd = frame.cmd->get_vulkan_data();

    // Capture if scene content is present before any transitions to determine the load operation
    const bool scene_content_present = swapchain_image.get_layout() == vk::ImageLayout::eColorAttachmentOptimal;

    if (auto* const draw_data = ImGui::GetDrawData(); nullptr != draw_data && draw_data->TotalVtxCount > 0) {
        // Transition swapchain image to colour attachment optimal if needed
        swapchain_image.transit(vk_cmd, image::TransitionRequest::color_attachment());

        // Begin dynamic rendering
        vk::RenderingAttachmentInfo color_attachment;
        color_attachment.imageView = swapchain_view.get_vulkan_data();
        color_attachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        // If a scene was blitted (came from COLOR_ATTACHMENT_OPTIMAL), use LOAD to preserve the content.
        color_attachment.loadOp = scene_content_present ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eClear;
        color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
        color_attachment.clearValue.color = vk::ClearColorValue { std::array { 0.0f, 0.0f, 0.0f, 1.0f } };

        vk::RenderingInfo rendering_info;
        rendering_info.renderArea.extent.width = swapchain_view.get_extent().width;
        rendering_info.renderArea.extent.height = swapchain_view.get_extent().height;
        rendering_info.layerCount = 1;
        rendering_info.setColorAttachments(color_attachment);

        vk_cmd.beginRendering(rendering_info);

        // `RenderDrawData` may call `ImGui_ImplVulkan_UpdateTexture`, which internally
        // does `vkQueueSubmit` + `vkQueueWaitIdle` on the queue handle we handed ImGui
        // at init. Take the queue's submission mutex here so those ops are serialised
        // with every other submit / present path in the backend.
        const std::lock_guard _ { queue::Queue::get().get_submission_lock() };
        ImGui_ImplVulkan_RenderDrawData(draw_data, static_cast<VkCommandBuffer>(vk_cmd));

        vk_cmd.endRendering();
    }

    // Transition swapchain image to present
    swapchain_image.transit(vk_cmd, image::TransitionRequest::present());
}

#endif

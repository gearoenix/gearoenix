#include "gx-vk-imgui-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "device/gx-vk-dev-physical.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "engine/gx-vk-eng-engine.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-instance.hpp"
#include "pipeline/gx-vk-pip-cache.hpp"
#include "queue/gx-vk-qu-graph.hpp"
#include "queue/gx-vk-qu-queue.hpp"
#include "sync/gx-vk-sync-fence.hpp"
#include "pipeline/gx-vk-pip-manager.hpp"
#include "gx-vk-swapchain.hpp"
#include "command/gx-vk-cmd-manager.hpp"

#include <imgui/backends/imgui_impl_vulkan.h>

gearoenix::vulkan::ImGuiManager::ImGuiManager()
    : cmds(core::Singleton<engine::Engine>::get().get_graphed_queue()->place_node_between(queue::NodeLabel::Start, queue::NodeLabel::ImGUI, queue::NodeLabel::End, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT))
{
    const auto& e = core::Singleton<engine::Engine>::get();

    ImGui_ImplVulkan_InitInfo info {};
    info.ApiVersion = Instance::get().get_api_version();
    info.Instance = Instance::get().get_vulkan_data();
    info.PhysicalDevice = device::Physical::get().get_vulkan_data();
    info.Device = device::Logical::get().get_vulkan_data();
    info.QueueFamily = device::Physical::get().get_graphics_queue_node_index();
    info.Queue = e.get_graphed_queue()->get_q().get_vulkan_data();
    info.DescriptorPool = descriptor::Manager::get().get_imgui()->get_vulkan_data();
    info.RenderPass = e.get_render_pass()->get_vulkan_data();
    info.MinImageCount = static_cast<decltype(info.MinImageCount)>(Swapchain::get().get_image_views().size());
    info.ImageCount = info.MinImageCount;
    info.PipelineCache = pipeline::Manager::get().get_cache()->get_vulkan_data();
#if GX_DEBUG_MODE
    info.CheckVkResultFn = +[](const VkResult result) {
        GX_VK_CHK(result);
    };
#endif
    ImGui_ImplVulkan_LoadFunctions(Instance::get().get_api_version(), +[](const char* const name, void*) {
        return Loader::get(name);
    });
    ImGui_ImplVulkan_Init(&info);
    upload_fonts();
}

gearoenix::vulkan::ImGuiManager::~ImGuiManager()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();
}

void gearoenix::vulkan::ImGuiManager::upload_fonts()
{
    command::Buffer cmd = command::Manager::get().create(command::Type::Primary);
    cmd.begin();
    // TODO
    // ImGui_ImplVulkan_CreateFontsTexture(cmd.get_vulkan_data());
    cmd.end();
    sync::Fence fence;
    queue::Queue q;
    q.submit(cmd, fence);
    fence.wait();
    // TODO
    // ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void gearoenix::vulkan::ImGuiManager::start_frame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
    (void)this;
}

void gearoenix::vulkan::ImGuiManager::end_frame()
{
    ImGui::Render();
    (void)this;
}

void gearoenix::vulkan::ImGuiManager::update()
{
    ImDrawData* draw_data = ImGui::GetDrawData();
    // TODO
    // auto& cmd = *cmds[e.get_frame_number()];
    // cmd.begin();
    // cmd.begin(e.get_render_pass(), e.get_current_framebuffer()); // TODO
    // ImGui_ImplVulkan_RenderDrawData(draw_data, cmd.get_vulkan_data());
    // cmd.end_render_pass();
    // cmd.end();
}

#endif
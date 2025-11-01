#include "gx-vk-imgui-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "descriptor/gx-vk-des-pool.hpp"
#include "engine/gx-vk-eng-engine.hpp"
#include "gx-vk-check.hpp"
#include "pipeline/gx-vk-pip-cache.hpp"
#include "queue/gx-vk-qu-graph.hpp"
#include "queue/gx-vk-qu-queue.hpp"
#include "sync/gx-vk-sync-fence.hpp"
#include <imgui/backends/imgui_impl_vulkan.h>

gearoenix::vulkan::ImGuiManager::ImGuiManager(engine::Engine& e)
    : e(e)
    , cmds(e.get_graphed_queue()->place_node_between(queue::NodeLabel::Start, queue::NodeLabel::ImGUI, queue::NodeLabel::End, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT))
{
    ImGui_ImplVulkan_InitInfo info {};
    info.Instance = e.get_instance().get_vulkan_data();
    info.PhysicalDevice = e.get_physical_device().get_vulkan_data();
    info.Device = e.get_logical_device().get_vulkan_data();
    info.QueueFamily = e.get_physical_device().get_graphics_queue_node_index();
    info.Queue = e.get_graphed_queue()->get_q().get_vulkan_data();
    info.PipelineCache = e.get_pipeline_manager().get_cache()->get_vulkan_data();
    info.DescriptorPool = e.get_descriptor_manager().get_imgui()->get_vulkan_data();
    info.MinImageCount = static_cast<decltype(info.MinImageCount)>(e.get_swapchain().get_image_views().size());
    info.ImageCount = info.MinImageCount;
#if GX_DEBUG_MODE
    info.CheckVkResultFn = +[](const VkResult result) {
        GX_VK_CHK(result);
    };
#endif
    ImGui_ImplVulkan_LoadFunctions(+[](const char* const name, void*) {
        return Loader::get(name);
    });
    ImGui_ImplVulkan_Init(&info, e.get_render_pass().get_vulkan_data());
    upload_fonts();
}

gearoenix::vulkan::ImGuiManager::~ImGuiManager()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();
}

void gearoenix::vulkan::ImGuiManager::upload_fonts()
{
    command::Buffer cmd = e.get_command_manager().create(command::Type::Primary);
    cmd.begin();
    ImGui_ImplVulkan_CreateFontsTexture(cmd.get_vulkan_data());
    cmd.end();
    sync::Fence fence(e.get_logical_device());
    queue::Queue q(e);
    q.submit(cmd, fence);
    fence.wait();
    ImGui_ImplVulkan_DestroyFontUploadObjects();
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
    auto& cmd = *cmds[e.get_frame_number()];
    cmd.begin();
    cmd.begin(e.get_render_pass(), e.get_current_framebuffer());
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd.get_vulkan_data());
    cmd.end_render_pass();
    cmd.end();
}

#endif
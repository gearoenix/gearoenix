#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-loader.hpp"

namespace gearoenix::vulkan {
struct ImGuiManager final {
private:
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VkFormat imgui_colour_format = VK_FORMAT_UNDEFINED;

public:
    ImGuiManager(ImGuiManager&&) = delete;
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(ImGuiManager&&) = delete;
    ImGuiManager& operator=(const ImGuiManager&&) = delete;

    ImGuiManager();
    ~ImGuiManager();

    void upload_fonts();
    void start_frame();
    void end_frame();
    void update();
};
}
#endif
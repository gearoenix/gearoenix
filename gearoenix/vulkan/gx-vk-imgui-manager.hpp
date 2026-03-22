#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-loader.hpp"

namespace gearoenix::vulkan {
struct ImGuiManager final {
private:
    vk::raii::DescriptorPool descriptor_pool { nullptr };
    vk::Format imgui_colour_format = vk::Format::eUndefined;

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

#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <memory>
#include <vector>

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan {
struct ImGuiManager final {
    constexpr static char NODE_NAME[] = "ui";

private:
    const std::vector<std::shared_ptr<command::Buffer>> cmds;

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
#ifndef GEAROENIX_VULKAN_IMGUI_HPP
#define GEAROENIX_VULKAN_IMGUI_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include <memory>
#include <vector>

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan {
struct ImGuiManager final {
    constexpr static const auto* const NODE_NAME = "ui";

private:
    engine::Engine& e;
    const std::vector<std::shared_ptr<command::Buffer>> cmds;

public:
    ImGuiManager(ImGuiManager&&) = delete;
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(ImGuiManager&&) = delete;
    ImGuiManager& operator=(const ImGuiManager&&) = delete;

    explicit ImGuiManager(engine::Engine& e) noexcept;
    ~ImGuiManager() noexcept;

    void upload_fonts() noexcept;
    void start_frame() noexcept;
    void end_frame() noexcept;
    void update() noexcept;
};
}

#endif
#endif
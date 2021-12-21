#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-loader.hpp"
#include <vector>

namespace gearoenix::dxr {
struct DescriptorManager;
struct Device;
struct Engine;
struct PipelineManager;
struct Queue;
struct Swapchain;
struct Command;

struct SubmissionManager final {
    GX_GET_RRF_PRV(Engine, e)
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)
    GX_GET_REFC_PRV(std::shared_ptr<DescriptorManager>, descriptor_manager)
    GX_GET_REFC_PRV(std::shared_ptr<PipelineManager>, pipeline_manager)
    GX_GET_REFC_PRV(std::shared_ptr<Swapchain>, swapchain)
    GX_GET_REFC_PRV(std::shared_ptr<Queue>, queue)

private:
    struct Frame final {
        std::vector<std::unique_ptr<Command>> threads_g_buffer_filler_commands;
        std::vector<ID3D12CommandList*> threads_g_buffer_filler_command_lists_raw;
    };

    std::uint64_t frame_number = 0;
    const std::uint64_t frames_count;
    std::vector<Frame> frames_data;

public:
    SubmissionManager(Engine& e) noexcept;
    ~SubmissionManager() noexcept;

    [[nodiscard]] bool render_frame() noexcept;
    void clear_command_lists() noexcept;
};
}

#endif
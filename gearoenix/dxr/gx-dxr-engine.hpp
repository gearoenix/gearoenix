#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-dxr-loader.hpp"

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::dxr {
struct Adapter;
struct DescriptorManager;
struct Device;
struct PipelineManager;
struct Queue;
struct SubmissionManager;
struct Swapchain;
struct Uploader;

struct Engine final : public render::engine::Engine {
    GX_GET_RRF_PRV(platform::Application, platform_application)
    GX_GET_CREF_PRV(std::shared_ptr<Adapter>, adapter)
    GX_GET_CREF_PRV(std::shared_ptr<Device>, device)
    GX_GET_CREF_PRV(std::shared_ptr<Queue>, queue)
    GX_GET_CREF_PRV(std::shared_ptr<Swapchain>, swapchain)
    GX_GET_CREF_PRV(std::shared_ptr<DescriptorManager>, descriptor_manager)
    GX_GET_CREF_PRV(std::shared_ptr<Uploader>, uploader)
    GX_GET_CREF_PRV(std::shared_ptr<PipelineManager>, pipeline_manager)
    GX_GET_CREF_PRV(std::shared_ptr<SubmissionManager>, submission_manager)

private:
    explicit Engine(platform::Application& platform_application) noexcept;
    void device_lost_handle(int failed_tries = 0) noexcept;
    void window_resized(int failed_tries = 0) noexcept;

public:
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    ~Engine() noexcept final;
    [[nodiscard]] static bool is_supported() noexcept;
    [[nodiscard]] static std::unique_ptr<Engine> construct(platform::Application& platform_application) noexcept;

    void start_frame() noexcept final;
    void end_frame() noexcept final;
    void upload_imgui_fonts() noexcept final;
};
}

#endif
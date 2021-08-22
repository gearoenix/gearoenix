#ifndef GEAROENIX_DIRECT3DX_ENGINE_HPP
#define GEAROENIX_DIRECT3DX_ENGINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../render/engine/gx-rnd-eng-engine.hpp"

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::direct3dx {
struct Adapter;
struct Device;
struct Queue;
struct Swapchain;
struct Engine final : public render::engine::Engine {
    GX_GET_RRF_PRV(platform::Application, platform_application)
    GX_GET_CREF_PRV(std::shared_ptr<Adapter>, adapter)
    GX_GET_CREF_PRV(std::shared_ptr<Device>, device)
    GX_GET_CREF_PRV(std::shared_ptr<Queue>, queue)
    GX_GET_CREF_PRV(std::shared_ptr<Swapchain>, swapchain)

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
#endif
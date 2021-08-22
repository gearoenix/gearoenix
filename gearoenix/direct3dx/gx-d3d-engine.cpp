#include "gx-d3d-engine.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-d3d-adapter.hpp"
#include "gx-d3d-device.hpp"
#include "gx-d3d-queue.hpp"
#include "gx-d3d-swapchain.hpp"
#include <dxgidebug.h>

gearoenix::direct3dx::Engine::Engine(platform::Application& platform_application) noexcept
    : render::engine::Engine(render::engine::Type::Direct3DX, platform_application)
    , platform_application(platform_application)
{
    device_lost_handle();
    GX_TODO
}

void gearoenix::direct3dx::Engine::device_lost_handle(const int failed_tries) noexcept
{
    GX_ASSERT(failed_tries < 3)

    swapchain = nullptr;
    queue = nullptr;
    device = nullptr;
    adapter = nullptr;

    {
        Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
    }

    adapter = std::make_shared<Adapter>();
    device = std::make_shared<Device>(adapter);
    queue = std::make_shared<Queue>(device);
    swapchain = std::make_shared<Swapchain>(queue);

    window_resized(failed_tries);
}

void gearoenix::direct3dx::Engine::window_resized(int failed_tries) noexcept
{
    if (swapchain->set_window_size(platform_application))
        device_lost_handle(++failed_tries);
}

gearoenix::direct3dx::Engine::~Engine() noexcept
{
    world = nullptr;
    GX_TODO
}

bool gearoenix::direct3dx::Engine::is_supported() noexcept
{
    GX_TODO
    return true;
}

std::unique_ptr<gearoenix::direct3dx::Engine> gearoenix::direct3dx::Engine::construct(
    platform::Application& platform_application) noexcept
{
    return std::unique_ptr<Engine>(new Engine(platform_application));
}

void gearoenix::direct3dx::Engine::start_frame() noexcept
{
    render::engine::Engine::start_frame();
}

void gearoenix::direct3dx::Engine::end_frame() noexcept
{
    render::engine::Engine::end_frame();
}

void gearoenix::direct3dx::Engine::upload_imgui_fonts() noexcept
{
    GX_TODO
}

#endif

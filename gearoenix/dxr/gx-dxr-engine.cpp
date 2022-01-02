#include "gx-dxr-engine.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "../platform/gx-plt-library.hpp"
#include "gx-dxr-adapter.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-descriptor.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-mesh.hpp"
#include "gx-dxr-model.hpp"
#include "gx-dxr-pipeline.hpp"
#include "gx-dxr-queue.hpp"
#include "gx-dxr-shader.hpp"
#include "gx-dxr-submission.hpp"
#include "gx-dxr-swapchain.hpp"
#include "gx-dxr-texture.hpp"
#include "gx-dxr-uploader.hpp"
#include <d3dx12.h>
#include <dxgidebug.h>
#include <fstream>
#include <string>

gearoenix::dxr::Engine::Engine(platform::Application& platform_application) noexcept
    : render::engine::Engine(render::engine::Type::Direct3DX, platform_application)
    , platform_application(platform_application)
{
    device_lost_handle();
}

void gearoenix::dxr::Engine::device_lost_handle(const int failed_tries) noexcept
{
    GX_ASSERT(failed_tries < 3)

    texture_manager = nullptr;
    submission_manager = nullptr;
    pipeline_manager = nullptr;
    mesh_manager = nullptr;
    uploader = nullptr;
    descriptor_manager = nullptr;
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
    queue = std::make_shared<Queue>(device, Queue::Type::Direct);
    swapchain = std::make_shared<Swapchain>(queue);
    descriptor_manager = std::make_shared<DescriptorManager>(device);
    uploader = std::make_shared<Uploader>(device);
    mesh_manager = std::make_unique<MeshManager>(*this);
    model_manager = std::make_unique<ModelManager>(*this);
    pipeline_manager = std::make_shared<PipelineManager>(device);
    submission_manager = std::make_shared<SubmissionManager>(*this);
    texture_manager = std::make_unique<TextureManager>(*this);

    window_resized(failed_tries);
}

void gearoenix::dxr::Engine::window_resized(int failed_tries) noexcept
{
    if (swapchain->set_window_size(platform_application))
        device_lost_handle(++failed_tries);
}

gearoenix::dxr::Engine::~Engine() noexcept
{
    swapchain->wait_for_gpu();
    world = nullptr;
}

bool gearoenix::dxr::Engine::is_supported() noexcept
{
    GX_TODO
    return true;
}

std::unique_ptr<gearoenix::dxr::Engine> gearoenix::dxr::Engine::construct(
    platform::Application& platform_application) noexcept
{
    return std::unique_ptr<Engine>(new Engine(platform_application));
}

void gearoenix::dxr::Engine::start_frame() noexcept
{
    render::engine::Engine::start_frame();
}

void gearoenix::dxr::Engine::end_frame() noexcept
{
    render::engine::Engine::end_frame();
    if (submission_manager->render_frame())
        device_lost_handle();
}

void gearoenix::dxr::Engine::upload_imgui_fonts() noexcept
{
    GX_TODO
}

#endif

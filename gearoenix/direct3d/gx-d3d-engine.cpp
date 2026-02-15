#include "gx-d3d-engine.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "../platform/gx-plt-library.hpp"
#include "gx-d3d-adapter.hpp"
#include "gx-d3d-camera.hpp"
#include "gx-d3d-check.hpp"
#include "gx-d3d-descriptor.hpp"
#include "gx-d3d-device.hpp"
#include "gx-d3d-mesh.hpp"
#include "gx-d3d-model.hpp"
#include "gx-d3d-pipeline.hpp"
#include "gx-d3d-queue.hpp"
#include "gx-d3d-shader.hpp"
#include "gx-d3d-submission.hpp"
#include "gx-d3d-swapchain.hpp"
#include "gx-d3d-texture.hpp"
#include "gx-d3d-uploader.hpp"
#include <d3dx12.h>
#include <dxgidebug.h>
#include <fstream>
#include <string>

gearoenix::d3d::Engine::Engine(platform::Application& platform_application)
    : render::engine::Engine(render::engine::Type::Direct3D, platform_application)
    , platform_application(platform_application)
{
    frames_count = GX_D3D_FRAMES_BACKBUFFER_NUMBER;
    device_lost_handle(0);
}

void gearoenix::d3d::Engine::device_lost_handle(const int failed_tries)
{
    GX_ASSERT(failed_tries < 3);

    texture_manager = nullptr;
    submission_manager = nullptr;
    pipeline_manager = nullptr;
    camera_manager = nullptr;
    model_manager = nullptr;
    mesh_manager = nullptr;
    uploader = nullptr;
    swapchain = nullptr;
    descriptor_manager = nullptr;
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
    descriptor_manager = std::make_shared<DescriptorManager>(device);
    swapchain = std::make_shared<Swapchain>(*this);
    uploader = std::make_shared<Uploader>(device);
    mesh_manager = std::make_unique<MeshManager>(*this);
    model_manager = std::make_unique<ModelManager>(*this);
    pipeline_manager = std::make_shared<PipelineManager>(*this);
    submission_manager = std::make_shared<SubmissionManager>(*this);
    texture_manager = std::make_unique<TextureManager>(*this);
    camera_manager = std::make_unique<CameraManager>(*this);

    window_resized(failed_tries);
}

void gearoenix::d3d::Engine::window_resized(int failed_tries)
{
    if (swapchain->set_window_size(platform_application))
        device_lost_handle(++failed_tries);
}

void gearoenix::d3d::Engine::window_resized() { window_resized(0); }

gearoenix::d3d::Engine::~Engine()
{
    swapchain->wait_for_gpu();
    world = nullptr;
}

bool gearoenix::d3d::Engine::is_supported()
{
    GX_TODO;
    return true;
}

std::unique_ptr<gearoenix::d3d::Engine> gearoenix::d3d::Engine::construct(platform::Application& platform_application) { return std::unique_ptr<Engine>(new Engine(platform_application)); }

void gearoenix::d3d::Engine::start_frame() { render::engine::Engine::start_frame(); }

void gearoenix::d3d::Engine::end_frame()
{
    render::engine::Engine::end_frame();
    if (submission_manager->render_frame())
        device_lost_handle(0);
}

void gearoenix::d3d::Engine::upload_imgui_fonts() { GX_TODO; }

#endif

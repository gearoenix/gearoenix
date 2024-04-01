#include "gx-mtl-engine.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../platform/gx-plt-application.hpp"
#import "gx-mtl-buffer.hpp"
#import "gx-mtl-camera.hpp"
#import "gx-mtl-heap.hpp"
#import "gx-mtl-mesh.hpp"
#import "gx-mtl-model.hpp"
#import "gx-mtl-pipeline.hpp"
#import "gx-mtl-submission.hpp"
#import "gx-mtl-texture.hpp"
#import "gx-mtl-uploader.hpp"

gearoenix::metal::Engine::Engine(platform::Application& platform_application)
    : render::engine::Engine(render::engine::Type::Metal, platform_application)
    , device(platform_application.get_app_delegate().view_controller.metal_kit_view.device)
    , pipeline_manager(new PipelineManager(*this))
    , uploader(new Uploader(*this))
    , heap_manager(new HeapManager(*this))
    , buffer_manager(new BufferManager(*this))
    , submission_manager(new SubmissionManager(*this))
{
    frames_count = GEAROENIX_METAL_FRAMES_COUNT;
    camera_manager = std::make_unique<CameraManager>(*this);
    mesh_manager = std::make_unique<MeshManager>(*this);
    model_manager = std::make_unique<ModelManager>(*this);
    texture_manager = std::make_unique<TextureManager>(*this);
}

gearoenix::metal::Engine::~Engine() = default;

void gearoenix::metal::Engine::start_frame()
{
    render::engine::Engine::start_frame();
}

void gearoenix::metal::Engine::end_frame()
{
    render::engine::Engine::end_frame();
    submission_manager->update();
}

void gearoenix::metal::Engine::window_resized()
{
}

void gearoenix::metal::Engine::upload_imgui_fonts()
{
}

bool gearoenix::metal::Engine::is_supported()
{
    return true;
}

std::unique_ptr<gearoenix::metal::Engine> gearoenix::metal::Engine::construct(platform::Application& platform_application)
{
    return std::make_unique<gearoenix::metal::Engine>(platform_application);
}

#endif

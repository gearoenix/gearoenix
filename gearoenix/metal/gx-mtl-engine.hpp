#ifndef GEAROENIX_METAL_ENGINE_HPP
#define GEAROENIX_METAL_ENGINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/macro/gx-cr-mcr-getter-setter.hpp"
#import "../render/engine/gx-rnd-eng-engine.hpp"
#import <Metal/MTLDevice.h>

namespace gearoenix::metal {
struct BufferManager;
struct PipelineManager;
struct Uploader;
struct HeapManager;
struct SubmissionManager;
struct Engine final : public render::engine::Engine {
    GX_GET_VAL_PRV(id<MTLDevice>, device, nil)
    GX_GET_UPTR_PRV(PipelineManager, pipeline_manager)
    GX_GET_UPTR_PRV(Uploader, uploader)
    GX_GET_UPTR_PRV(HeapManager, heap_manager)
    GX_GET_UPTR_PRV(BufferManager, buffer_manager)
    GX_GET_UPTR_PRV(SubmissionManager, submission_manager)

    Engine(platform::Application& platform_application);
    ~Engine() final;
    void start_frame() final;
    void end_frame() final;
    void window_resized() final;
    void upload_imgui_fonts() final;
    [[nodiscard]] static bool is_supported();
    [[nodiscard]] static std::unique_ptr<Engine> construct(platform::Application& platform_application);
};
}

#endif
#endif

#ifndef GEAROENIX_METAL_ENGINE_HPP
#define GEAROENIX_METAL_ENGINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../render/engine/gx-rnd-eng-engine.hpp"
#import "../core/macro/gx-cr-mcr-getter-setter.hpp"
#import <Metal/MTLDevice.h>

namespace gearoenix::metal {
struct PipelineManager;
struct Uploader;
struct HeapManager;
struct SubmissionManager;
    struct Engine final: public render::engine::Engine {
        GX_GET_VAL_PRV(id<MTLDevice>, device, nil)
        GX_GET_UPTR_PRV(PipelineManager, pipeline_manager)
        GX_GET_UPTR_PRV(Uploader, uploader)
        GX_GET_UPTR_PRV(HeapManager, heap_manager)
        GX_GET_UPTR_PRV(SubmissionManager, submission_manager)
        
        Engine(platform::Application& platform_application) noexcept;
        ~Engine() noexcept final;
        void start_frame() noexcept final;
        void end_frame() noexcept final;
        void window_resized() noexcept final;
        void upload_imgui_fonts() noexcept final;
        [[nodiscard]] static bool is_supported() noexcept;
        [[nodiscard]] static std::unique_ptr<Engine> construct(platform::Application& platform_application) noexcept;
    };
}

#endif
#endif

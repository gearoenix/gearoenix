#include "gx-mtl-pipeline.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import "../platform/gx-plt-application.hpp"
#import "../platform/stream/gx-plt-stm-asset.hpp"
#import "../platform/gx-plt-log.hpp"
#import <dispatch/data.h>

gearoenix::metal::PipelineManager::PipelineManager(Engine& e) noexcept
{
    std::unique_ptr<platform::stream::Asset> all_shaders_lib_asset(platform::stream::Asset::construct(e.get_platform_application(), "shaders/all-metal-shaders.metallib"));
    if(nullptr == all_shaders_lib_asset)
        GX_LOG_F("Can not load shader files.")
        
    const auto all_shaders_lib_file_content = all_shaders_lib_asset->get_file_content();
    all_shaders_lib = [e.get_device() newLibraryWithData:dispatch_data_create(all_shaders_lib_file_content.data(), all_shaders_lib_file_content.size(), nil, nil) error:nil];
    MTKView* view = e.get_platform_application().get_app_delegate().view_controller.metal_kit_view;
    id<MTLFunction> vertex_function = [all_shaders_lib newFunctionWithName:@"gbuffers_filler_vertex_shader"];
    id<MTLFunction> fragment_function = [all_shaders_lib newFunctionWithName:@"gbuffers_filler_fragment_shader"];
    MTLRenderPipelineDescriptor *pipeline_state_descriptor = [MTLRenderPipelineDescriptor new];
    pipeline_state_descriptor.label = @"RT Pipeline";
    pipeline_state_descriptor.sampleCount = view.sampleCount;
    pipeline_state_descriptor.vertexFunction = vertex_function;
    pipeline_state_descriptor.fragmentFunction = fragment_function;
    pipeline_state_descriptor.vertexDescriptor = vertex_descriptions.pbr;
    pipeline_state_descriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
    pipeline_state_descriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
    pipeline_state_descriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;
    NSError* error;
    gbuffers_filler_ps = [e.get_device() newRenderPipelineStateWithDescriptor:pipeline_state_descriptor error:&error];
    if(nil == gbuffers_filler_ps)
        GX_LOG_F("Failed to create gbuffers filler pipeline state: " << [error.localizedFailureReason UTF8String])
}

#endif

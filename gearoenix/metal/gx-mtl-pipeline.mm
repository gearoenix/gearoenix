#include "gx-mtl-pipeline.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import "../platform/gx-plt-application.hpp"
#import "../platform/stream/gx-plt-stm-asset.hpp"
#import <dispatch/data.h>

gearoenix::metal::PipelineManager::PipelineManager(Engine& e) noexcept
{
    std::unique_ptr<platform::stream::Asset> all_shaders_lib_asset(platform::stream::Asset::construct(e.get_platform_application(), "shaders/all-metal-shaders.metallib"));
    const auto all_shaders_lib_file_content = all_shaders_lib_asset->get_file_content();
    all_shaders_lib = [e.get_device() newLibraryWithData:dispatch_data_create(all_shaders_lib_file_content.data(), all_shaders_lib_file_content.size(), nil, nil) error:nil];
}

#endif

#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED

#include <memory>

namespace gearoenix::vulkan::pipeline {
struct Pipeline;

struct FormatPipelines final {
    std::shared_ptr<Pipeline> pbr_forward;
    std::shared_ptr<Pipeline> pbr_skinned_forward;
    std::shared_ptr<Pipeline> unlit_forward;
    std::shared_ptr<Pipeline> unlit_skinned_forward;
    std::shared_ptr<Pipeline> skybox_equirectangular;
    std::shared_ptr<Pipeline> skybox_cube;
};
}

#endif
#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <array>
#include <memory>

namespace gearoenix::vulkan::pipeline {
struct Pipeline;

enum struct FormatPipelinesIndices : std::uint8_t {
    pbr_forward_index = 0,
    pbr_skinned_forward_index = 1,
    unlit_forward_index = 2,
    unlit_skinned_forward_index = 3,
    skybox_equirectangular_index = 4,
    skybox_cube_index = 5,

    count = 6,
};

enum struct ShadowPipelinesIndices : std::uint8_t {
    no_skin_index = static_cast<std::uint8_t>(FormatPipelinesIndices::count),
    skinned_index = no_skin_index + 1,

    count = no_skin_index + 2,
};

struct FormatPipelinesDataHolder final {
    std::shared_ptr<Pipeline> pbr_forward;
    std::shared_ptr<Pipeline> pbr_skinned_forward;
    std::shared_ptr<Pipeline> unlit_forward;
    std::shared_ptr<Pipeline> unlit_skinned_forward;
    std::shared_ptr<Pipeline> skybox_equirectangular;
    std::shared_ptr<Pipeline> skybox_cube;
};

using AllPipelines = std::array<vk::Pipeline, static_cast<std::size_t>(ShadowPipelinesIndices::count)>;
}

#endif
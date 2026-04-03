#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include "../material/gx-vk-mat-draw-cache.hpp"

namespace gearoenix::vulkan::mesh {
struct DrawCache final {
    vk::DeviceSize vertex_offset = ~0ul;
    vk::DeviceSize index_offset = ~0ul;
    std::uint32_t indices_count = ~0u;

    material::DrawCache material_draw_cache;
};
}

#endif
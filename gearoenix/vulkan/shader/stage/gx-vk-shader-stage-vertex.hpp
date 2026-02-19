#pragma once
#include "../../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-shader-stage.hpp"

namespace gearoenix::vulkan::shader::stage {
struct Vertex final : Stage {
    explicit Vertex(std::span<std::uint8_t> data);
    ~Vertex() override;
};
}
#endif
#include "gx-vk-shader-stage-vertex.hpp"
#if GX_RENDER_VULKAN_ENABLED

gearoenix::vulkan::shader::stage::Vertex::Vertex(const std::span<std::uint8_t> data)
    : Stage(data)
{
}

gearoenix::vulkan::shader::stage::Vertex::~Vertex() = default;

#endif

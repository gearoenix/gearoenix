#include "gx-vk-shader-stage-fragment.hpp"
#if GX_RENDER_VULKAN_ENABLED

gearoenix::vulkan::shader::stage::Fragment::Fragment(const std::span<std::uint8_t> data)
    : Stage(data)
{
}

gearoenix::vulkan::shader::stage::Fragment::~Fragment() = default;

#endif

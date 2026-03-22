#include "gx-vk-shader-stage.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::shader::stage::Stage::Stage(const std::span<std::uint8_t> data)
    : vulkan_data(device::Logical::get().get_device(), vk::ShaderModuleCreateInfo { { }, data.size(), reinterpret_cast<const std::uint32_t*>(data.data()) })
{
}

gearoenix::vulkan::shader::stage::Stage::~Stage() = default;
#endif

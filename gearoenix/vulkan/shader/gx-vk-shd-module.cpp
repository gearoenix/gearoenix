#include "gx-vk-shd-module.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::shader::Module::Module(const std::span<const std::uint8_t> file_content)
    : vulkan_data(device::Logical::get().get_device(), vk::ShaderModuleCreateInfo { { }, file_content.size(), reinterpret_cast<const std::uint32_t*>(file_content.data()) })
{
}

gearoenix::vulkan::shader::Module::~Module() = default;

#endif

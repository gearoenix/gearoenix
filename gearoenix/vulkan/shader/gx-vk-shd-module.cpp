#include "gx-vk-shd-module.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::shader::Module::Module(const std::span<std::uint8_t> file_content)
{
    VkShaderModuleCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = static_cast<std::uint32_t>(file_content.size());
    info.pCode = reinterpret_cast<const std::uint32_t*>(file_content.data());
    GX_VK_CHK(vkCreateShaderModule(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::shader::Module::~Module()
{
    vkDestroyShaderModule(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

#endif

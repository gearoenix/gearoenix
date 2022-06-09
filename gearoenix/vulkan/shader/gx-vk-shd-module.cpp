#include "gx-vk-shd-module.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/stream/gx-plt-stm-local.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::shader::Module::Module(
    const engine::Engine& e,
    const std::string& file_address) noexcept
    : logical_device(e.get_logical_device())
{
    platform::stream::Local file(e.get_platform_application(), file_address);
    const auto file_content = file.get_file_content();
    VkShaderModuleCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = static_cast<std::uint32_t>(file_content.size());
    info.pCode = reinterpret_cast<const std::uint32_t*>(file_content.data());
    GX_VK_CHK(vkCreateShaderModule(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::shader::Module::~Module() noexcept
{
    vkDestroyShaderModule(logical_device.get_vulkan_data(), vulkan_data, nullptr);
}

#endif

#include "gx-vk-shader-stage.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../device/gx-vk-dev-logical.hpp"
#include "../../gx-vk-check.hpp"

gearoenix::vulkan::shader::stage::Stage::Stage(const std::span<std::uint8_t> data)
    : vulkan_data([&] {
        VkShaderModule vulkan_data;

        VkShaderModuleCreateInfo module_create_info;
        GX_SET_ZERO(module_create_info);
        module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        module_create_info.codeSize = data.size();
        module_create_info.pCode = reinterpret_cast<const std::uint32_t*>(data.data());
        GX_VK_CHK(vkCreateShaderModule(device::Logical::get().get_vulkan_data(), &module_create_info, nullptr, &vulkan_data));

        return vulkan_data;
    }())
{
}

gearoenix::vulkan::shader::stage::Stage::~Stage()
{
    vkDestroyShaderModule(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}
#endif

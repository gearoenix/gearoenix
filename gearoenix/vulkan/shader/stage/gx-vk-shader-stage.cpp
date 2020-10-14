#include "gx-vk-shader-stage.hpp"
#ifdef USE_VULKAN
#include "../../../core/gx-cr-static.hpp"
#include "../../../render/gx-rnd-engine.hpp"
#include "../../../render/shader/stage/gx-rnd-shd-stg-stage.hpp"
#include "../../device/gx-vk-dev-logical.hpp"
#include "../../device/gx-vk-dev-physical.hpp"
#include "../../gx-vk-check.hpp"
#include "../../gx-vk-instance.hpp"

gearoenix::render::shader::stage::Stage::Stage(std::vector<unsigned char>& data, Engine* engine)
    : device(engine->get_logical_device())
{
    VkShaderModuleCreateInfo module_create_info;
    setz(module_create_info);
    module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_create_info.codeSize = data.size();
    module_create_info.pCode = static_cast<const std::uint32_t*>(data.data());
    auto l = device->get_physical_device()->get_instance()->get_linker();
    VKC(l->vkCreateShaderModule(device->get_vulkan_data(), &module_create_info, nullptr, &vulkan_data));
}

gearoenix::render::shader::stage::Stage::~Stage()
{
    auto l = device->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyShaderModule(device->get_vulkan_data(), vulkan_data, nullptr);
}

VkShaderModule gearoenix::render::shader::stage::Stage::get_vulkan_data() const
{
    return vulkan_data;
}
#endif

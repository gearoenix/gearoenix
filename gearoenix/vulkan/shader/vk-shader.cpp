#include "vk-shader.hpp"
#include "../../core/static.hpp"
#include "../../system/sys-file.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

void gearoenix::nufrag::render::shader::Shader::delete_module(const VkShaderModule &module) {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyShaderModule(logical_device->get_vulkan_data(), module, nullptr);
}

void gearoenix::nufrag::render::shader::Shader::fetch(std::shared_ptr<system::File> &file, std::vector<uint8_t> &data) {
    uint16_t size;
    file->read(size);
    data.resize(size);
    file->read(data);
}

VkShaderModule gearoenix::nufrag::render::shader::Shader::load(std::shared_ptr<system::File> &file) {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    std::vector<uint8_t> spirv;
    fetch(file, spirv);
    LOGI(std::string("shader size is: ") + std::to_string(spirv.size()));
    VkShaderModuleCreateInfo module_create_info;
    setz(module_create_info);
    module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_create_info.codeSize = spirv.size();
    module_create_info.pCode = reinterpret_cast<uint32_t *>(spirv.data());
    VkShaderModule shader_module;
    VKC(l->vkCreateShaderModule(logical_device->get_vulkan_data(), &module_create_info, nullptr, &shader_module));
    return shader_module;
}

gearoenix::nufrag::render::shader::Shader::Shader(const std::shared_ptr<device::Logical> &logical_device): logical_device(logical_device) {

}

gearoenix::nufrag::render::shader::Shader::~Shader() {

}

#include "gx-vk-shd-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-shd-module.hpp"

gearoenix::vulkan::shader::Manager::Manager(const engine::Engine& e)
    : e(e)
{
}

gearoenix::vulkan::shader::Manager::~Manager() = default;

std::shared_ptr<gearoenix::vulkan::shader::Module> gearoenix::vulkan::shader::Manager::get(
    const std::string& shader_name)
{
    auto& search = modules[shader_name];
    auto result = search.lock();
    if (nullptr != result)
        return result;
    const auto file_address = "vkr-shaders/gx-vk-shd-" + shader_name + ".spv";
    result = std::make_shared<Module>(e, file_address);
    search = result;
    return result;
}

#endif

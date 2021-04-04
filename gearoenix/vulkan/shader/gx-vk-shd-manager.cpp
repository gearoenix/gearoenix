#include "gx-vk-shd-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-shd-module.hpp"

gearoenix::vulkan::shader::Manager::Manager(const engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::vulkan::shader::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::vulkan::shader::Module> gearoenix::vulkan::shader::Manager::get(
    const std::string& shader_name) noexcept
{
    auto& search = modules[shader_name];
    auto result = search.lock();
    if (nullptr != result)
        return result;
    /// TODO this has to be changed in future
    const auto file_address = "/home/thany/Projects/assets/" + shader_name + ".spv";
    result = std::make_shared<Module>(e, file_address);
    search = result;
    return result;
}

#endif

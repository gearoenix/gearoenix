#include "gx-vk-shd-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-shd-module.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"

gearoenix::vulkan::shader::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::vulkan::shader::Manager::~Manager() = default;

std::shared_ptr<gearoenix::vulkan::shader::Module> gearoenix::vulkan::shader::Manager::get(const std::string& shader_name)
{
    auto& search = modules[shader_name];
    if (auto result = search.lock(); nullptr != result) {
        return result;
    }
    const auto file_address = "vulkan/shader/" + shader_name + ".spv";
    const std::unique_ptr<platform::stream::Stream> stream(platform::stream::Asset::construct(file_address));
    GX_ASSERT_D(stream);
    const auto file_content = stream->get_file_content();
    auto result = std::make_shared<Module>(file_content);
    search = result;
    return result;
}

#endif

#include "gx-vk-lt-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"

#include <atomic>

namespace {
std::vector<GxShaderDataScene> shader_datas;
std::atomic<std::uint32_t> shader_data_last_index = 0;
std::shared_ptr<gearoenix::vulkan::buffer::Uniform> uniform_buffer;
}

gearoenix::vulkan::light::Manager::Manager()
    : Singleton<Manager>(this)
{
}

gearoenix::vulkan::light::Manager::~Manager() = default;

const gearoenix::vulkan::buffer::Uniform& gearoenix::vulkan::light::Manager::get_uniform_buffer()
{
    return *uniform_buffer;
}

#endif

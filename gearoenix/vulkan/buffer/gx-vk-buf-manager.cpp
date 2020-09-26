#include "gx-vk-buf-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../engine/gx-vk-eng-engine.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "gx-vk-buf-uniform.hpp"

gearoenix::vulkan::buffer::Manager::Manager(
    std::shared_ptr<memory::Manager> mm,
    engine::Engine* eng) noexcept
    : render::buffer::Manager(eng)
    , memory_manager(std::move(mm))
{
}

gearoenix::vulkan::buffer::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform(const std::size_t size) noexcept
{
    VkBufferCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = static_cast<VkDeviceSize>(size);
    info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    auto [buff, mem, data] = memory_manager->create(info, memory::Usage::CpuToGpu);
    return std::make_shared<Uniform>(size, buff, std::move(mem), data, dynamic_cast<engine::Engine*>(e));
}
#endif

#include "gx-vk-buf-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../engine/gx-vk-eng-engine.hpp"
#include "../memory/gx-vk-mem-manager.hpp"

gearoenix::vulkan::buffer::Manager::Manager(
    std::shared_ptr<memory::Manager> mm,
    engine::Engine* eng) noexcept
    : render::buffer::Manager(eng)
    , memory_manager(std::move(mm))
{
}

gearoenix::vulkan::buffer::Manager::~Manager() noexcept = default;

gearoenix::render::buffer::Uniform* gearoenix::vulkan::buffer::Manager::create_uniform(const std::size_t size) noexcept
{
    return nullptr;
}
#endif

#include "gx-vk-mem-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../../math/gx-math-numeric.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"
#include "gx-vk-mem-memory.hpp"

gearoenix::vulkan::memory::Manager::Manager(std::shared_ptr<device::Logical> ld) noexcept
    : logical_device(std::move(ld))
{
}

std::shared_ptr<gearoenix::vulkan::memory::Manager> gearoenix::vulkan::memory::Manager::construct(
    std::shared_ptr<device::Logical> logical_device) noexcept
{
    std::shared_ptr<Manager> mgr(new Manager(std::move(logical_device)));
    mgr->self = mgr;
    return mgr;
}

gearoenix::vulkan::memory::Manager::~Manager() noexcept
{
    memories.clear();
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Manager::allocate(
    const std::size_t size, const std::uint32_t type_bits, const Place place) noexcept
{
    const auto& physical_device = logical_device->get_physical_device();
    const auto memory_properties = place == Place::Gpu ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    const auto index = physical_device->get_memory_type_index(type_bits, memory_properties);
    auto mem = memories[index].lock();
    if (nullptr == mem) {
        const auto& cfg = physical_device->get_surface()->get_system_application()->get_configuration().get_render();
        const auto sz = place == Place::Gpu ? cfg.get_maximum_gpu_render_memory_size() : cfg.get_maximum_cpu_render_memory_size();
        mem = Memory::construct(sz, index, self.lock());
        memories[index] = mem;
    }
    return mem->allocate(size);
}
#endif

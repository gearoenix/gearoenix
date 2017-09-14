#include "vk-img-image.hpp"
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../memory/vk-mem-memory.hpp"
#include "../memory/vk-mem-pool.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

gearoenix::render::image::Image::Image(
    const std::shared_ptr<device::Logical>& logical_device,
    const VkImage& vulkan_data, const std::shared_ptr<memory::Memory>& mem)
    : logical_device(logical_device)
    , vulkan_data(vulkan_data)
    , mem(mem)
{
}

gearoenix::render::image::Image::Image(
    const std::shared_ptr<device::Logical>& logical_device,
    const VkImageCreateInfo& info, const std::shared_ptr<memory::Pool>& pool)
    : logical_device(logical_device)
{
    auto l = logical_device->get_physical_device()->get_instance()->get_linker();
    VKC(l->vkCreateImage(logical_device->get_vulkan_data(), &info, nullptr,
        &vulkan_data));
    VkMemoryRequirements mem_requirements;
    setz(mem_requirements);
    l->vkGetImageMemoryRequirements(logical_device->get_vulkan_data(),
        vulkan_data, &mem_requirements);
    mem = std::shared_ptr<memory::Memory>(
        new memory::Memory(pool, mem_requirements));
    VKC(l->vkBindImageMemory(logical_device->get_vulkan_data(), vulkan_data,
        mem->get_vulkan_data(), 0));
}

gearoenix::render::image::Image::~Image()
{
    if (mem != nullptr) {
        auto l = logical_device->get_physical_device()->get_instance()->get_linker();
        l->vkDestroyImage(logical_device->get_vulkan_data(), vulkan_data, nullptr);
    }
}

const VkImage& gearoenix::render::image::Image::get_vulkan_data() const
{
    return vulkan_data;
}

const std::shared_ptr<gearoenix::render::device::Logical>& gearoenix::render::image::Image::get_logical_device() const
{
    return logical_device;
}

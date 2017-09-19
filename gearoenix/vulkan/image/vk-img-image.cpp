#include "vk-img-image.hpp"
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../memory/vk-mem-manager.hpp"
#include "../memory/vk-mem-memory.hpp"
#include "../memory/vk-mem-sub-memory.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

//gearoenix::render::image::Image::Image(
//    const std::shared_ptr<device::Logical>& logical_device,
//    const VkImage& vulkan_data, const std::shared_ptr<memory::Memory>& mem)
//    : logical_device(logical_device)
//    , vulkan_data(vulkan_data)
//    , mem(mem)
//{
//}

gearoenix::render::image::Image::Image(device::Logical* logical_device, const VkImageCreateInfo& info, memory::Manager* mem_mgr)
    : logical_device(logical_device)
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    const VkDevice vk_dev = logical_device->get_vulkan_data();
    VKC(l->vkCreateImage(vk_dev, &info, nullptr, &vulkan_data));
    VkMemoryRequirements mem_requirements;
    setz(mem_requirements);
    l->vkGetImageMemoryRequirements(vk_dev, vulkan_data, &mem_requirements);
    if (mem_mgr == nullptr) {
        mem = new memory::Memory(logical_device, mem_requirements);
        VKC(l->vkBindImageMemory(vk_dev, vulkan_data, mem->get_vulkan_data(), 0));
    } else {
        LOGF("TODO");
    }
}

gearoenix::render::image::Image::~Image()
{
    if (mem != nullptr || submem != nullptr) {
        const device::Physical* p = logical_device->get_physical_device();
        const Linker* l = p->get_instance()->get_linker();
        l->vkDestroyImage(logical_device->get_vulkan_data(), vulkan_data, nullptr);
    }
    if (mem != nullptr)
        delete mem;
    if (submem != nullptr)
        delete submem;
}

const VkImage& gearoenix::render::image::Image::get_vulkan_data() const
{
    return vulkan_data;
}

const gearoenix::render::device::Logical* gearoenix::render::image::Image::get_logical_device() const
{
    return logical_device;
}

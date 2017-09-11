#include "vk-mem-memory.hpp"
#include "vk-mem-pool.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../image/vk-img-image.hpp"
#include "../vk-instance.hpp"
#include "../vk-check.hpp"
#include "../../core/cr-static.hpp"

gearoenix::render::memory::Memory::Memory(const std::shared_ptr<Pool> &pool, const VkMemoryRequirements &req, const VkMemoryPropertyFlagBits &properties) : pool(pool) {
    auto &d = pool->get_logical_device();
    auto &p = d->get_physical_device();
    auto &l = p->get_instance()->get_linker();
    VkMemoryAllocateInfo mem_alloc;
    setz(mem_alloc);
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.allocationSize = req.size;
    mem_alloc.memoryTypeIndex = p->get_memory_type_index(req.memoryTypeBits, properties);
    VKC(l->vkAllocateMemory(d->get_vulkan_data(), &mem_alloc, nullptr, &vulkan_data));
}

gearoenix::render::memory::Memory::~Memory() {
    auto &d = pool->get_logical_device();
    auto &l = d->get_physical_device()->get_instance()->get_linker();
    l->vkFreeMemory(d->get_vulkan_data(), vulkan_data, nullptr);
}

const VkDeviceMemory &gearoenix::render::memory::Memory::get_vulkan_data() const {
    return vulkan_data;
}

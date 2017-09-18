//#include "../../core/cr-build-configuration.hpp"
//#ifdef USE_VULKAN
//#include "../../core/cr-static.hpp"
//#include "../device/vk-dev-logical.hpp"
//#include "../device/vk-dev-physical.hpp"
//#include "../memory/vk-mem-memory.hpp"
//#include "../memory/vk-mem-manager.hpp"
//#include "../vk-check.hpp"
//#include "../vk-instance.hpp"
//#include "../vk-linker.hpp"
//#include "vk-buf-uniform.hpp"
//gearoenix::render::buffer::Uniform::Uniform(std::shared_ptr<memory::Pool>& pool,
//    int data_size)
//    : logical_device(pool->get_logical_device())
//    , data_size(static_cast<VkDeviceSize>(data_size))
//{
//    auto p = logical_device->get_physical_device();
//    auto l = p->get_instance()->get_linker();
//    VkMemoryRequirements mem_reqs;
//    setz(mem_reqs);
//    VkBufferCreateInfo buffer_info;
//    setz(buffer_info);
//    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    buffer_info.size = this->data_size;
//    buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
//    VKC(l->vkCreateBuffer(logical_device->get_vulkan_data(), &buffer_info,
//        nullptr, &vulkan_data));
//    l->vkGetBufferMemoryRequirements(logical_device->get_vulkan_data(),
//        vulkan_data, &mem_reqs);
//    // TODO:take care of this comment
//    // Get the memory type index that supports host visibile memory access
//    // Most implementations offer multiple memory types and selecting the correct
//    // one to
//    // allocate memory from is crucial We also want the buffer to be host coherent
//    // so we don't
//    // have to flush (or sync after every update.
//    // Note: This may affect performance so you might not want to do this in a
//    // real world
//    // application that updates buffers on a regular base
//    mem = std::shared_ptr<memory::Memory>(new memory::Memory(
//        pool, mem_reqs, static_cast<VkMemoryPropertyFlagBits>(
//                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
//    VKC(l->vkBindBufferMemory(logical_device->get_vulkan_data(), vulkan_data,
//        mem->get_vulkan_data(), 0));
//}

//gearoenix::render::buffer::Uniform::~Uniform()
//{
//    auto l = logical_device->get_physical_device()->get_instance()->get_linker();
//    l->vkDestroyBuffer(logical_device->get_vulkan_data(), vulkan_data, nullptr);
//}

//void gearoenix::render::buffer::Uniform::update(const void* data)
//{
//    auto l = logical_device->get_physical_device()->get_instance()->get_linker();
//    void* buffer_data;
//    VKC(l->vkMapMemory(logical_device->get_vulkan_data(), mem->get_vulkan_data(),
//        0, data_size, 0, &buffer_data));
//    std::memcpy(buffer_data, data, data_size);
//    // Unmap after data has been copied
//    // Note: Since we requested a host coherent memory type for the uniform
//    // buffer,
//    // the write is instantly visible to the GPU
//    l->vkUnmapMemory(logical_device->get_vulkan_data(), mem->get_vulkan_data());
//}

//const std::shared_ptr<gearoenix::render::device::Logical>& gearoenix::render::buffer::Uniform::get_logical_device() const
//{
//    return logical_device;
//}

//const std::shared_ptr<gearoenix::render::memory::Memory>& gearoenix::render::buffer::Uniform::get_memory() const
//{
//    return mem;
//}

//const VkBuffer& gearoenix::render::buffer::Uniform::get_vulkan_data() const
//{
//    return vulkan_data;
//}

//const VkDeviceSize& gearoenix::render::buffer::Uniform::get_data_size() const
//{
//    return data_size;
//}

//#endif

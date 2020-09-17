#define VMA_IMPLEMENTATION
#include "gx-vk-img-image.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "../memory/gx-vk-mem-sub-memory.hpp"

gearoenix::vulkan::image::Image::Image(
    std::shared_ptr<device::Logical> ld,
    VkImage vulkan_data,
    std::shared_ptr<memory::Memory> mm) noexcept
    : logical_device(std::move(ld))
    , mem(std::move(mm))
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::image::Image::Image(
    std::shared_ptr<device::Logical> ld,
    const VkImageCreateInfo& info,
    std::shared_ptr<memory::Manager> mem_mgr) noexcept
    : logical_device(std::move(ld))
    , image_width(info.extent.width)
    , image_height(info.extent.height)
    , format(info.format)
{
    const auto vk_dev = logical_device->get_vulkan_data();
    GX_VK_CHK(vkCreateImage(vk_dev, &info, nullptr, &vulkan_data))
    VkMemoryRequirements mem_requirements;
    GX_SET_ZERO(mem_requirements)
    Loader::vkGetImageMemoryRequirements(vk_dev, vulkan_data, &mem_requirements);
    vmaCreateImage() if (mem_mgr == nullptr)
    {
        mem = std::make_shared<memory::Memory>(logical_device, mem_requirements);
        GX_VK_CHK(vkBindImageMemory(vk_dev, vulkan_data, mem->get_vulkan_data(), 0))
    }
    else
    {
        submem = mem_mgr->create_submemory(mem_requirements.size);
        VKC(l->vkBindImageMemory(vk_dev, vulkan_data, submem->get_memory()->get_vulkan_data(), submem->get_offset()));
    }
}

gearoenix::vulkan::image::Image::~Image()
{
    if (mem != nullptr || submem != nullptr) {
        const device::Physical* p = logical_device->get_physical_device();
        const Linker* l = p->get_instance()->get_linker();
        l->vkDestroyImage(logical_device->get_vulkan_data(), vulkan_data, nullptr);
        if (mem != nullptr)
            delete mem;
        if (submem != nullptr)
            delete submem;
    }
}

const VkImage& gearoenix::vulkan::image::Image::get_vulkan_data() const
{
    return vulkan_data;
}

const gearoenix::vulkan::device::Logical* gearoenix::vulkan::image::Image::get_logical_device() const
{
    return logical_device;
}

void gearoenix::vulkan::image::Image::transit(command::Buffer* c, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt)
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;
    VkImageMemoryBarrier barrier;
    setz(barrier);
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_lyt;
    barrier.newLayout = new_lyt;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vulkan_data;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    if (old_lyt == VK_IMAGE_LAYOUT_UNDEFINED && new_lyt == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_lyt == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_lyt == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        LOGF("Unexpected");
    }
    l->vkCmdPipelineBarrier(
        c->get_vulkan_data(),
        src_stage, dst_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

void gearoenix::vulkan::image::Image::transit_for_writing(command::Buffer* c)
{
    transit(c, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

void gearoenix::vulkan::image::Image::copy_from_buffer(command::Buffer* c, buffer::SubBuffer* b)
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    const buffer::Buffer* buf = b->get_buffer();
    VkBufferImageCopy region;
    setz(region);
    region.bufferOffset = b->get_offset();
    //    switch (fmt) {
    //    case VK_FORMAT_R8G8B8A8_UNORM:
    //        region.bufferRowLength = 4 * img_width;
    //        break;
    //    case VK_FORMAT_R8G8B8_UNORM:
    //        region.bufferRowLength = 3 * img_width;
    //        break;
    //    default:
    //        LOGF("Unexpected");
    //    }
    //    region.bufferImageHeight = img_height;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        img_width,
        img_height,
        1
    };
    l->vkCmdCopyBufferToImage(
        c->get_vulkan_data(),
        buf->get_vulkan_data(),
        vulkan_data,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

void gearoenix::vulkan::image::Image::transit_for_reading(command::Buffer* c)
{
    transit(c, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkFormat gearoenix::vulkan::image::Image::get_format() const
{
#ifdef DEBUG_MODE
    if (fmt == VK_FORMAT_UNDEFINED) {
        LOGF("Unexpected");
    }
#endif
    return fmt;
}
#endif

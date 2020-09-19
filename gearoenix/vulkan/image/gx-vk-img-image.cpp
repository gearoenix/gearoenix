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
#include "../memory/gx-vk-mem-vma.hpp"

gearoenix::vulkan::image::Image::Image(
    std::shared_ptr<memory::Manager> mem_mgr,
    VkImage vulkan_data,
    std::shared_ptr<memory::Memory> mm) noexcept
    : logical_device(mem_mgr->get_logical_device())
    , memory_manager(std::move(mem_mgr))
    , allocated_memory(std::move(mm))
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::image::Image::Image(
    std::uint32_t image_width,
    std::uint32_t image_height,
    VkFormat format,
    std::shared_ptr<memory::Manager> mem_mgr) noexcept
    : logical_device(mem_mgr->get_logical_device())
    , memory_manager(std::move(mem_mgr))
    , image_width(image_width)
    , image_height(image_height)
    , format(format)
{
    VkImageCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.extent.width = image_width;
    info.extent.height = image_height;
    std::tie(vulkan_data, allocated_memory) = memory_manager->create_image(info);
}

gearoenix::vulkan::image::Image::~Image() noexcept
{
    if (nullptr != allocated_memory)
        memory_manager->destroy_image(vulkan_data, allocated_memory);
}

void gearoenix::vulkan::image::Image::transit(command::Buffer& c, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt) noexcept
{
    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;
    VkImageMemoryBarrier barrier;
    GX_SET_ZERO(barrier)
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
        GXLOGF("Unexpected layouts.")
    }
    Loader::vkCmdPipelineBarrier(
        c.get_vulkan_data(),
        src_stage, dst_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

void gearoenix::vulkan::image::Image::transit_for_writing(command::Buffer& c) noexcept
{
    transit(c, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

void gearoenix::vulkan::image::Image::copy_from_buffer(command::Buffer& c, buffer::Buffer& b) noexcept
{
    VkBufferImageCopy region;
    GX_SET_ZERO(region)
    region.bufferOffset = b.get_offset();
    switch (format) {
    case VK_FORMAT_R8G8B8A8_UNORM:
        region.bufferRowLength = 4 * image_width;
        break;
    case VK_FORMAT_R8G8B8_UNORM:
        region.bufferRowLength = 3 * image_width;
        break;
    default:
        GXLOGF("Unexpected format")
    }
    region.bufferImageHeight = image_height;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        image_width,
        image_height,
        1
    };
    Loader::vkCmdCopyBufferToImage(
        c.get_vulkan_data(),
        b.get_vulkan_data(),
        vulkan_data,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

void gearoenix::vulkan::image::Image::transit_for_reading(command::Buffer& c) noexcept
{
    transit(c, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
#endif

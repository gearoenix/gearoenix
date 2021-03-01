#include "gx-vk-img-image.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../memory/gx-vk-mem-manager.hpp"

void gearoenix::vulkan::image::Image::terminate() noexcept
{
    if (nullptr != allocated_memory && vulkan_data != nullptr) {
        vkDestroyImage(logical_device->get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

gearoenix::vulkan::image::Image::Image(Image&& o) noexcept
    : logical_device(o.logical_device)
    , allocated_memory(std::move(o.allocated_memory))
    , image_width(o.image_width)
    , image_height(o.image_height)
    , image_depth(o.image_depth)
    , mipmap_level(o.mipmap_level)
    , array_layers(o.array_layers)
    , format(o.format)
    , flags(o.flags)
    , usage(o.usage)
    , vulkan_data(o.vulkan_data)
{
    o.vulkan_data = nullptr;
}

gearoenix::vulkan::image::Image::Image(
    const device::Logical* const logical_device,
    const std::uint32_t image_width,
    const std::uint32_t image_height,
    const std::uint32_t image_depth,
    const std::uint32_t mipmap_level,
    const std::uint32_t array_layers,
    const VkFormat format,
    const VkImageCreateFlags flags,
    const VkImageUsageFlags usage,
    VkImage vulkan_data) noexcept
    : logical_device(logical_device)
    , image_width(image_width)
    , image_height(image_height)
    , image_depth(image_depth)
    , mipmap_level(mipmap_level)
    , array_layers(array_layers)
    , format(format)
    , flags(flags)
    , usage(usage)
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::image::Image::Image(
    const std::uint32_t image_width,
    const std::uint32_t image_height,
    const std::uint32_t image_depth,
    const std::uint32_t mipmap_level,
    const std::uint32_t array_layers,
    const VkFormat format,
    const VkImageCreateFlags flags,
    const VkImageUsageFlags usage,
    memory::Manager& mem_mgr) noexcept
    : logical_device(&mem_mgr.get_e().get_logical_device())
    , image_width(image_width)
    , image_height(image_height)
    , image_depth(image_depth)
    , mipmap_level(mipmap_level)
    , array_layers(array_layers)
    , format(format)
    , flags(flags)
    , usage(usage)
{
    VkImageCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = image_depth <= 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
    info.format = format;
    info.extent.width = image_width;
    info.extent.height = image_height;
    info.extent.depth = image_depth;
    info.mipLevels = mipmap_level;
    info.arrayLayers = array_layers;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage;
    info.flags = flags;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    GX_VK_CHK(vkCreateImage(logical_device->get_vulkan_data(), &info, nullptr, &vulkan_data))
    VkMemoryRequirements mem_req;
    GX_SET_ZERO(mem_req)
    vkGetImageMemoryRequirements(logical_device->get_vulkan_data(), vulkan_data, &mem_req);
    allocated_memory = mem_mgr.allocate(mem_req.size, mem_req.memoryTypeBits, memory::Place::Gpu);
    GX_VK_CHK(vkBindImageMemory(
        logical_device->get_vulkan_data(),
        vulkan_data,
        allocated_memory->get_vulkan_data(),
        allocated_memory->get_allocator()->get_offset()))
}

gearoenix::vulkan::image::Image::~Image() noexcept
{
    terminate();
}

gearoenix::vulkan::image::Image& gearoenix::vulkan::image::Image::operator=(Image&& o) noexcept
{
    terminate();
    logical_device = o.logical_device;
    allocated_memory = std::move(o.allocated_memory);
    image_width = o.image_width;
    image_height = o.image_height;
    image_depth = o.image_depth;
    mipmap_level = o.mipmap_level;
    array_layers = o.array_layers;
    format = o.format;
    flags = o.flags;
    usage = o.usage;
    vulkan_data = o.vulkan_data;
    o.vulkan_data = nullptr;
    return *this;
}
//
//void gearoenix::vulkan::image::Image::transit(command::Buffer& c, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt) noexcept
//{
//    VkPipelineStageFlags src_stage;
//    VkPipelineStageFlags dst_stage;
//    VkImageMemoryBarrier barrier;
//    GX_SET_ZERO(barrier)
//    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//    barrier.oldLayout = old_lyt;
//    barrier.newLayout = new_lyt;
//    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.image = vulkan_data;
//    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    barrier.subresourceRange.baseMipLevel = 0;
//    barrier.subresourceRange.levelCount = 1;
//    barrier.subresourceRange.baseArrayLayer = 0;
//    barrier.subresourceRange.layerCount = 1;
//    if (old_lyt == VK_IMAGE_LAYOUT_UNDEFINED && new_lyt == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//        barrier.srcAccessMask = 0;
//        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//    } else if (old_lyt == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_lyt == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//    } else {
//        GX_LOG_F("Unexpected layouts.")
//    }
//    Loader::vkCmdPipelineBarrier(
//        c.get_vulkan_data(),
//        src_stage, dst_stage,
//        0,
//        0, nullptr,
//        0, nullptr,
//        1, &barrier);
//}
//
//void gearoenix::vulkan::image::Image::transit_for_writing(command::Buffer& c) noexcept
//{
//    transit(c, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//}
//
//void gearoenix::vulkan::image::Image::copy_from_buffer(command::Buffer& c, const buffer::Buffer& b) noexcept
//{
//    VkBufferImageCopy region;
//    GX_SET_ZERO(region)
//    region.bufferOffset = b.get_allocator()->get_root_offset();
//    switch (format) {
//    case VK_FORMAT_R8G8B8A8_UNORM:
//        region.bufferRowLength = 4 * image_width;
//        break;
//    case VK_FORMAT_R8G8B8_UNORM:
//        region.bufferRowLength = 3 * image_width;
//        break;
//    case VK_FORMAT_R32G32B32A32_SFLOAT:
//        region.bufferRowLength = 4 * 4 * image_width;
//        break;
//    default:
//        GX_LOG_F("Unexpected format")
//    }
//    region.bufferImageHeight = image_height;
//    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    region.imageSubresource.mipLevel = 0;
//    region.imageSubresource.baseArrayLayer = 0;
//    region.imageSubresource.layerCount = 1;
//    region.imageOffset = { 0, 0, 0 };
//    region.imageExtent = {
//        image_width,
//        image_height,
//        1
//    };
//    Loader::vkCmdCopyBufferToImage(
//        c.get_vulkan_data(),
//        b.get_vulkan_data(),
//        vulkan_data,
//        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//        1,
//        &region);
//}
//
//void gearoenix::vulkan::image::Image::transit_for_reading(command::Buffer& c) noexcept
//{
//    transit(c, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//}

#endif

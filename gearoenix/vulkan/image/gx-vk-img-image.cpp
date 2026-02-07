#include "gx-vk-img-image.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-flagger.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "../memory/gx-vk-mem-manager.hpp"

namespace {
[[nodiscard]] std::pair<VkAccessFlags, VkPipelineStageFlags> get_layout_access_and_stage(const VkImageLayout layout)
{
    switch (layout) {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        return { 0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        return { VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT };
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        return { VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT };
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        return { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        return { VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        return {
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
        };
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        return { 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
    default:
        GX_UNEXPECTED;
    }
}
}

gearoenix::vulkan::image::Image::Image(
    const std::string& name,
    const std::uint32_t image_width,
    const std::uint32_t image_height,
    const std::uint32_t image_depth,
    const VkImageType image_type,
    const std::uint32_t mipmap_levels,
    const std::uint32_t array_layers,
    const VkFormat format,
    const VkImageCreateFlags flags,
    const VkImageUsageFlags usage,
    const VkImage vulkan_data)
    : image_width(image_width)
    , image_height(image_height)
    , image_depth(image_depth)
    , image_type(image_type)
    , mipmap_levels(mipmap_levels)
    , array_layers(array_layers)
    , format(format)
    , flags(flags)
    , usage(usage)
    , vulkan_data(vulkan_data)
{
    GX_VK_MARK(name, vulkan_data);
}

gearoenix::vulkan::image::Image::Image(
    const std::string& name,
    const std::uint32_t image_width,
    const std::uint32_t image_height,
    const std::uint32_t image_depth,
    const VkImageType image_type,
    const std::uint32_t mipmap_levels,
    const std::uint32_t array_layers,
    const VkFormat format,
    const VkImageCreateFlags flags,
    const VkImageUsageFlags usage)
    : image_width(image_width)
    , image_height(image_height)
    , image_depth(image_depth)
    , image_type(image_type)
    , mipmap_levels(mipmap_levels)
    , array_layers(array_layers)
    , format(format)
    , flags(flags)
    , usage(usage)
{
    auto& logical_device = device::Logical::get();

    VkImageCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = image_type;
    info.format = format;
    info.extent.width = image_width;
    info.extent.height = image_height;
    info.extent.depth = image_depth;
    info.mipLevels = mipmap_levels;
    info.arrayLayers = array_layers;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage;
    info.flags = flags;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    GX_VK_CHK(vkCreateImage(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data));

    VkMemoryRequirements mem_req;
    GX_SET_ZERO(mem_req);
    vkGetImageMemoryRequirements(logical_device.get_vulkan_data(), vulkan_data, &mem_req);

    allocated_memory = memory::Manager::get().allocate(static_cast<std::int64_t>(mem_req.size), mem_req.memoryTypeBits, memory::Place::Gpu);
    GX_ASSERT_D(allocated_memory != nullptr);
    GX_VK_CHK(vkBindImageMemory(logical_device.get_vulkan_data(), vulkan_data, allocated_memory->get_vulkan_data(), allocated_memory->get_allocator()->get_offset()));

    GX_VK_MARK(name, vulkan_data);
}

gearoenix::vulkan::image::Image::~Image()
{
    if (owned) {
        vkDestroyImage(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
    }
}

VkImageAspectFlags gearoenix::vulkan::image::Image::get_aspect_flags() const
{
    if (!GX_FLAG_CHECK(usage, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
    switch (format) {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_FORMAT_S8_UINT:
        return VK_IMAGE_ASPECT_STENCIL_BIT;
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
        GX_UNEXPECTED;
    }
}

void gearoenix::vulkan::image::Image::transit(const VkCommandBuffer cmd, const VkImageLayout new_layout)
{
    if (layout == new_layout) {
        return;
    }

    const auto [src_access, src_stage] = get_layout_access_and_stage(layout);
    const auto [dst_access, dst_stage] = get_layout_access_and_stage(new_layout);

    VkImageMemoryBarrier barrier;
    GX_SET_ZERO(barrier);
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = src_access;
    barrier.dstAccessMask = dst_access;
    barrier.oldLayout = layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vulkan_data;
    barrier.subresourceRange.aspectMask = get_aspect_flags();
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipmap_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = array_layers;

    vkCmdPipelineBarrier(
        cmd,
        src_stage, dst_stage,
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    layout = new_layout;
}

void gearoenix::vulkan::image::Image::transit_mips(
    const VkCommandBuffer cmd,
    const VkImageLayout old_layout,
    const VkImageLayout new_layout,
    const std::uint32_t base_mip_level,
    const std::uint32_t level_count) const
{
    if (old_layout == new_layout) {
        return;
    }

    const auto [src_access, src_stage] = get_layout_access_and_stage(old_layout);
    const auto [dst_access, dst_stage] = get_layout_access_and_stage(new_layout);

    VkImageMemoryBarrier barrier;
    GX_SET_ZERO(barrier);
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = src_access;
    barrier.dstAccessMask = dst_access;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vulkan_data;
    barrier.subresourceRange.aspectMask = get_aspect_flags();
    barrier.subresourceRange.baseMipLevel = base_mip_level;
    barrier.subresourceRange.levelCount = level_count;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = array_layers;

    vkCmdPipelineBarrier(
        cmd,
        src_stage, dst_stage,
        0, 0, nullptr, 0, nullptr, 1, &barrier);
}

#endif

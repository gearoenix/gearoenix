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

bool gearoenix::vulkan::image::Image::PerMipState::operator==(const PerMipState& other) const
{
    return layout == other.layout
        && queue_family_index == other.queue_family_index
        && access == other.access
        && stage == other.stage;
}

bool gearoenix::vulkan::image::Image::PerArrayState::operator==(const PerArrayState& other) const
{
    return per_mip_states == other.per_mip_states;
}

bool gearoenix::vulkan::image::Image::State::is_uniform() const
{
    if (per_array_states.empty()) {
        return true;
    }
    const auto& first = per_array_states[0].per_mip_states[0];
    for (const auto& array_state : per_array_states) {
        for (const auto& mip_state : array_state.per_mip_states) {
            if (!(mip_state == first)) {
                return false;
            }
        }
    }
    return true;
}

bool gearoenix::vulkan::image::Image::State::has_uniform_layout() const
{
    if (per_array_states.empty()) {
        return true;
    }

    const auto first = per_array_states[0].per_mip_states[0].layout;
    for (const auto& [per_mip_states] : per_array_states) {
        for (const auto& mip_state : per_mip_states) {
            if (first != mip_state.layout) {
                return false;
            }
        }
    }

    return true;
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::shader_read(const VkPipelineStageFlags2 stage)
{
    return {
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .access = VK_ACCESS_2_SHADER_READ_BIT,
        .stage = stage,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::transfer_dst()
{
    return {
        .layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .access = VK_ACCESS_2_TRANSFER_WRITE_BIT,
        .stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::transfer_src()
{
    return {
        .layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .access = VK_ACCESS_2_TRANSFER_READ_BIT,
        .stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::color_attachment()
{
    return {
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .access = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .stage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::depth_attachment()
{
    return {
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .stage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::present()
{
    return {
        .layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .access = VK_ACCESS_2_NONE,
        .stage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::with_mips(
    const std::uint32_t base, const std::uint32_t count) const
{
    auto result = *this;
    result.base_mip = base;
    result.mip_count = count;
    return result;
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::with_layers(
    const std::uint32_t base, const std::uint32_t count) const
{
    auto result = *this;
    result.base_layer = base;
    result.layer_count = count;
    return result;
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::with_queue_family(
    const std::uint32_t family) const
{
    auto result = *this;
    result.queue_family = family;
    return result;
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
    , state { .per_array_states = std::vector(array_layers, PerArrayState { .per_mip_states = std::vector<PerMipState>(mipmap_levels) }) }
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
    , state { .per_array_states = std::vector(array_layers, PerArrayState { .per_mip_states = std::vector<PerMipState>(mipmap_levels) }) }
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

VkImageLayout gearoenix::vulkan::image::Image::get_layout() const
{
    GX_ASSERT_D(!state.per_array_states.empty());
    GX_ASSERT_D(!state.per_array_states[0].per_mip_states.empty());
    GX_ASSERT_D(state.has_uniform_layout());
    return state.per_array_states[0].per_mip_states[0].layout;
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

void gearoenix::vulkan::image::Image::transit(const VkCommandBuffer cmd, const TransitionRequest& request)
{
    const auto actual_mip_count = request.mip_count == VK_REMAINING_MIP_LEVELS
        ? mipmap_levels - request.base_mip
        : request.mip_count;
    const auto actual_layer_count = request.layer_count == VK_REMAINING_ARRAY_LAYERS
        ? array_layers - request.base_layer
        : request.layer_count;

    thread_local std::vector<VkImageMemoryBarrier2> barriers;
    barriers.clear();
    barriers.reserve(actual_mip_count * actual_layer_count);

    for (std::uint32_t layer = request.base_layer; layer < request.base_layer + actual_layer_count; ++layer) {
        for (std::uint32_t mip = request.base_mip; mip < request.base_mip + actual_mip_count; ++mip) {
            auto& [layout, queue_family_index, access, stage] = state.per_array_states[layer].per_mip_states[mip];

            if (layout == request.layout && queue_family_index == request.queue_family && access == request.access && stage == request.stage) {
                continue;
            }

            VkImageMemoryBarrier2 barrier;
            GX_SET_ZERO(barrier);
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask = stage;
            barrier.srcAccessMask = access;
            barrier.dstStageMask = request.stage;
            barrier.dstAccessMask = request.access;
            barrier.oldLayout = layout;
            barrier.newLayout = request.layout;
            barrier.srcQueueFamilyIndex = queue_family_index;
            barrier.dstQueueFamilyIndex = request.queue_family;
            barrier.image = vulkan_data;
            barrier.subresourceRange.aspectMask = get_aspect_flags();
            barrier.subresourceRange.baseMipLevel = mip;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = layer;
            barrier.subresourceRange.layerCount = 1;

            barriers.push_back(barrier);

            layout = request.layout;
            access = request.access;
            stage = request.stage;
            queue_family_index = request.queue_family;
        }
    }

    if (barriers.empty()) {
        return;
    }

    VkDependencyInfo dependency_info;
    GX_SET_ZERO(dependency_info);
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.imageMemoryBarrierCount = static_cast<std::uint32_t>(barriers.size());
    dependency_info.pImageMemoryBarriers = barriers.data();

    vkCmdPipelineBarrier2(cmd, &dependency_info);
}

#endif

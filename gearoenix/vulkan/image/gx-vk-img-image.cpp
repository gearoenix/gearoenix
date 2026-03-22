#include "gx-vk-img-image.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../memory/gx-vk-mem-memory.hpp"

bool gearoenix::vulkan::image::Image::PerMipState::operator==(const PerMipState& other) const
{
    return layout == other.layout && queue_family_index == other.queue_family_index && access == other.access && stage == other.stage;
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

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::shader_read(const vk::PipelineStageFlags2 stage)
{
    return {
        .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .access = vk::AccessFlagBits2::eShaderRead,
        .stage = stage,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::transfer_dst()
{
    return {
        .layout = vk::ImageLayout::eTransferDstOptimal,
        .access = vk::AccessFlagBits2::eTransferWrite,
        .stage = vk::PipelineStageFlagBits2::eTransfer,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::transfer_src()
{
    return {
        .layout = vk::ImageLayout::eTransferSrcOptimal,
        .access = vk::AccessFlagBits2::eTransferRead,
        .stage = vk::PipelineStageFlagBits2::eTransfer,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::color_attachment()
{
    return {
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
        .access = vk::AccessFlagBits2::eColorAttachmentWrite,
        .stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::depth_attachment()
{
    return {
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        .access = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        .stage = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::present()
{
    return {
        .layout = vk::ImageLayout::ePresentSrcKHR,
        .access = vk::AccessFlagBits2::eNone,
        .stage = vk::PipelineStageFlagBits2::eBottomOfPipe,
    };
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::with_mips(const std::uint32_t base, const std::uint32_t count) const
{
    auto result = *this;
    result.base_mip = base;
    result.mip_count = count;
    return result;
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::with_layers(const std::uint32_t base, const std::uint32_t count) const
{
    auto result = *this;
    result.base_layer = base;
    result.layer_count = count;
    return result;
}

gearoenix::vulkan::image::TransitionRequest gearoenix::vulkan::image::TransitionRequest::with_queue_family(const std::uint32_t family) const
{
    auto result = *this;
    result.queue_family = family;
    return result;
}

gearoenix::vulkan::image::Image::Image(const std::string& name, const std::uint32_t image_width, const std::uint32_t image_height, const std::uint32_t image_depth, const vk::ImageType image_type, const std::uint32_t mipmap_levels,
    const std::uint32_t array_layers, const vk::Format format, const vk::ImageCreateFlags flags, const vk::ImageUsageFlags usage, const vk::Image vulkan_data)
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

gearoenix::vulkan::image::Image::Image(const std::string& name, const std::uint32_t image_width, const std::uint32_t image_height, const std::uint32_t image_depth, const vk::ImageType image_type, const std::uint32_t mipmap_levels,
    const std::uint32_t array_layers, const vk::Format format, const vk::ImageCreateFlags flags, const vk::ImageUsageFlags usage)
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
    auto dev = device::Logical::get().get_vulkan_data();

    const vk::ImageCreateInfo info {
        flags, image_type, format,
        { image_width, image_height, image_depth },
        mipmap_levels, array_layers,
        vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage
    };
    vulkan_data = dev.createImage(info);

    auto mem_req = dev.getImageMemoryRequirements(vulkan_data);

    allocated_memory = memory::Manager::get().allocate(
        static_cast<std::int64_t>(mem_req.size), static_cast<std::int64_t>(mem_req.alignment), mem_req.memoryTypeBits, memory::Place::Gpu);
    GX_ASSERT_D(allocated_memory != nullptr);
    dev.bindImageMemory(vulkan_data, allocated_memory->get_vulkan_data(), allocated_memory->get_allocator()->get_offset());

    GX_VK_MARK(name, vulkan_data);
}

gearoenix::vulkan::image::Image::~Image()
{
    if (owned) {
        device::Logical::get().get_vulkan_data().destroyImage(vulkan_data);
    }
}

vk::ImageLayout gearoenix::vulkan::image::Image::get_layout() const
{
    GX_ASSERT_D(!state.per_array_states.empty());
    GX_ASSERT_D(!state.per_array_states[0].per_mip_states.empty());
    GX_ASSERT_D(state.has_uniform_layout());
    return state.per_array_states[0].per_mip_states[0].layout;
}

vk::ImageAspectFlags gearoenix::vulkan::image::Image::get_aspect_flags() const
{
    if (!(usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)) {
        return vk::ImageAspectFlagBits::eColor;
    }
    switch (format) {
    case vk::Format::eD16Unorm:
    case vk::Format::eD32Sfloat:
    case vk::Format::eX8D24UnormPack32:
        return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eS8Uint:
        return vk::ImageAspectFlagBits::eStencil;
    case vk::Format::eD16UnormS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    default:
        GX_UNEXPECTED;
    }
}

void gearoenix::vulkan::image::Image::generate_mipmaps(const vk::CommandBuffer cmd)
{
    const auto aspect_flags = get_aspect_flags();

    auto mip_width = static_cast<std::int32_t>(image_width);
    auto mip_height = static_cast<std::int32_t>(image_height);
    auto mip_depth = static_cast<std::int32_t>(image_depth);

    for (std::uint32_t mip = 1; mip < mipmap_levels; ++mip) {
        // Transition previous mip level to TRANSFER_SRC
        transit(cmd, TransitionRequest::transfer_src().with_mips(mip - 1, 1));
        // Transition destination mip level to TRANSFER_DST
        transit(cmd, TransitionRequest::transfer_dst().with_mips(mip, 1));

        vk::ImageBlit blit;
        blit.srcOffsets[0] = vk::Offset3D { 0, 0, 0 };
        blit.srcOffsets[1] = vk::Offset3D { mip_width, mip_height, mip_depth };
        blit.srcSubresource.aspectMask = aspect_flags;
        blit.srcSubresource.mipLevel = mip - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = array_layers;

        const auto next_width = std::max(1, mip_width >> 1);
        const auto next_height = std::max(1, mip_height >> 1);
        const auto next_depth = std::max(1, mip_depth >> 1);

        blit.dstOffsets[0] = vk::Offset3D { 0, 0, 0 };
        blit.dstOffsets[1] = vk::Offset3D { next_width, next_height, next_depth };
        blit.dstSubresource.aspectMask = aspect_flags;
        blit.dstSubresource.mipLevel = mip;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = array_layers;

        cmd.blitImage(vulkan_data, vk::ImageLayout::eTransferSrcOptimal, vulkan_data, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eLinear);

        // Transition the source mip to SHADER_READ_ONLY
        transit(cmd, TransitionRequest::shader_read().with_mips(mip - 1, 1));

        mip_width = next_width;
        mip_height = next_height;
        mip_depth = next_depth;
    }

    // Transition last mip level to SHADER_READ_ONLY
    transit(cmd, TransitionRequest::shader_read().with_mips(mipmap_levels - 1, 1));
}

void gearoenix::vulkan::image::Image::transit(const vk::CommandBuffer cmd, const TransitionRequest& request)
{
    const auto actual_mip_count = request.mip_count == vk::RemainingMipLevels ? mipmap_levels - request.base_mip : request.mip_count;
    const auto actual_layer_count = request.layer_count == vk::RemainingArrayLayers ? array_layers - request.base_layer : request.layer_count;

    thread_local std::vector<vk::ImageMemoryBarrier2> barriers;
    barriers.clear();
    barriers.reserve(actual_mip_count * actual_layer_count);

    for (std::uint32_t layer = request.base_layer; layer < request.base_layer + actual_layer_count; ++layer) {
        for (std::uint32_t mip = request.base_mip; mip < request.base_mip + actual_mip_count; ++mip) {
            auto& [layout, queue_family_index, access, stage] = state.per_array_states[layer].per_mip_states[mip];

            if (layout == request.layout && queue_family_index == request.queue_family && access == request.access && stage == request.stage) {
                continue;
            }

            vk::ImageMemoryBarrier2 barrier;
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

    vk::DependencyInfo dependency_info;
    dependency_info.setImageMemoryBarriers(barriers);
    cmd.pipelineBarrier2(dependency_info);
}

#endif

#include "gx-vk-img-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "gx-vk-img-image.hpp"

gearoenix::vulkan::image::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::vulkan::image::Manager::~Manager() = default;

void gearoenix::vulkan::image::Manager::upload(
    std::shared_ptr<Image>&& img,
    std::vector<std::vector<std::shared_ptr<buffer::Buffer>>>&& buffs,
    const bool generate_mipmaps,
    core::job::EndCaller<>&& end)
{
    // No depth upload, obviously!
    buffer::Manager::get().get_uploader()->push([img, buffs = std::move(buffs), end, generate_mipmaps]() mutable {
        auto cmd = command::Manager::get().create(command::Type::Primary);
        cmd->begin();

        const auto mip_levels = img->get_mipmap_levels();
        const auto array_layers = img->get_array_layers();
        const auto provided_mip_count = buffs.empty() ? 0u : static_cast<std::uint32_t>(buffs[0].size());
        const bool needs_mip_generation = generate_mipmaps && provided_mip_count == 1 && mip_levels > 1;

        VkImageMemoryBarrier barrier;
        GX_SET_ZERO(barrier);
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = img->get_vulkan_data();
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = array_layers;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(
            cmd->get_vulkan_data(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier);

        std::uint32_t array_index = 0;
        for (const auto& array_buffs : buffs) {
            std::uint32_t mip_index = 0;
            auto mip_width = img->get_image_width();
            auto mip_height = img->get_image_height();
            auto mip_depth = img->get_image_depth();
            for (const auto& mip_buff : array_buffs) {
                VkBufferImageCopy region;
                GX_SET_ZERO(region);
                region.bufferOffset = 0;
                region.bufferRowLength = 0;
                region.bufferImageHeight = 0;
                region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel = mip_index;
                region.imageSubresource.baseArrayLayer = array_index;
                region.imageSubresource.layerCount = 1;
                region.imageOffset = { 0, 0, 0 };
                region.imageExtent = { mip_width, mip_height, mip_depth };
                vkCmdCopyBufferToImage(cmd->get_vulkan_data(), mip_buff->get_vulkan_data(), img->get_vulkan_data(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                ++mip_index;
                mip_width = std::max(1u, mip_width >> 1);
                mip_height = std::max(1u, mip_height >> 1);
                mip_depth = std::max(1u, mip_depth >> 1);
            }
            ++array_index;
        }

        if (needs_mip_generation) {
            auto mip_width = static_cast<std::int32_t>(img->get_image_width());
            auto mip_height = static_cast<std::int32_t>(img->get_image_height());
            auto mip_depth = static_cast<std::int32_t>(img->get_image_depth());

            for (std::uint32_t mip = 1; mip < mip_levels; ++mip) {
                // Transition previous mip level to TRANSFER_SRC
                barrier.subresourceRange.baseMipLevel = mip - 1;
                barrier.subresourceRange.levelCount = 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(
                    cmd->get_vulkan_data(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                    0, 0, nullptr, 0, nullptr, 1, &barrier);

                VkImageBlit blit;
                GX_SET_ZERO(blit);
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mip_width, mip_height, mip_depth };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = mip - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = array_layers;

                const auto next_width = std::max(1, mip_width >> 1);
                const auto next_height = std::max(1, mip_height >> 1);
                const auto next_depth = std::max(1, mip_depth >> 1);

                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { next_width, next_height, next_depth };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = mip;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = array_layers;

                vkCmdBlitImage(
                    cmd->get_vulkan_data(),
                    img->get_vulkan_data(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    img->get_vulkan_data(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit,
                    VK_FILTER_LINEAR);

                // Transition the source mip to SHADER_READ_ONLY
                barrier.subresourceRange.baseMipLevel = mip - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(
                    cmd->get_vulkan_data(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    0, 0, nullptr, 0, nullptr, 1, &barrier);

                mip_width = next_width;
                mip_height = next_height;
                mip_depth = next_depth;
            }

            // Transition last mip level to SHADER_READ_ONLY
            barrier.subresourceRange.baseMipLevel = mip_levels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
                cmd->get_vulkan_data(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0, 0, nullptr, 0, nullptr, 1, &barrier);
        } else {
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mip_levels;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(
                cmd->get_vulkan_data(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        cmd->end();

        auto fence = std::make_shared<sync::Fence>();

        buffer::Manager::get().get_uploader_queue()->submit(*cmd, *fence);

        buffer::Manager::get().get_uploader()->push([cmd = std::move(cmd), fence = std::move(fence), buffs = std::move(buffs), end = std::move(end)] {
            fence->wait();
        });
    });
}
#endif
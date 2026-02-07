#include "gx-vk-img-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
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
    auto cmd = command::Manager::create_thread_independent();
    cmd->begin();

    const auto vk_cmd = cmd->get_vulkan_data();
    const auto mip_levels = img->get_mipmap_levels();
    const auto array_layers = img->get_array_layers();
    const auto provided_mip_count = buffs.empty() ? 0u : static_cast<std::uint32_t>(buffs[0].size());
    const bool needs_mip_generation = generate_mipmaps && provided_mip_count == 1 && mip_levels > 1;

    const auto aspect_flags = img->get_aspect_flags();

    // Transition the entire image to TRANSFER_DST_OPTIMAL for upload
    img->transit(vk_cmd, TransitionRequest::transfer_dst());

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
            region.imageSubresource.aspectMask = aspect_flags;
            region.imageSubresource.mipLevel = mip_index;
            region.imageSubresource.baseArrayLayer = array_index;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { mip_width, mip_height, mip_depth };
            vkCmdCopyBufferToImage(vk_cmd, mip_buff->get_vulkan_data(), img->get_vulkan_data(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

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
            img->transit(vk_cmd, TransitionRequest::transfer_src().with_mips(mip - 1, 1));

            VkImageBlit blit;
            GX_SET_ZERO(blit);
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mip_width, mip_height, mip_depth };
            blit.srcSubresource.aspectMask = aspect_flags;
            blit.srcSubresource.mipLevel = mip - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = array_layers;

            const auto next_width = std::max(1, mip_width >> 1);
            const auto next_height = std::max(1, mip_height >> 1);
            const auto next_depth = std::max(1, mip_depth >> 1);

            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { next_width, next_height, next_depth };
            blit.dstSubresource.aspectMask = aspect_flags;
            blit.dstSubresource.mipLevel = mip;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = array_layers;

            vkCmdBlitImage(
                vk_cmd,
                img->get_vulkan_data(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                img->get_vulkan_data(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            // Transition the source mip to SHADER_READ_ONLY
            img->transit(vk_cmd, TransitionRequest::shader_read().with_mips(mip - 1, 1));

            mip_width = next_width;
            mip_height = next_height;
            mip_depth = next_depth;
        }

        // Transition last mip level to SHADER_READ_ONLY
        img->transit(vk_cmd, TransitionRequest::shader_read().with_mips(mip_levels - 1, 1));
    } else {
        // Transition the entire image to SHADER_READ_ONLY
        img->transit(vk_cmd, TransitionRequest::shader_read());
    }

    cmd->end();

    auto fence = std::make_shared<sync::Fence>();

    queue::Queue::get().submit(*cmd, *fence);

    core::job::send_job_to_pool([cmd = std::move(cmd), fence = std::move(fence), buffs = std::move(buffs), end = std::move(end)] {
        fence->wait();
        (void)cmd;
        (void)buffs;
        (void)end;
    });
}
#endif
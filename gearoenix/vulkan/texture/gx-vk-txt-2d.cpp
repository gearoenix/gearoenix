#include "gx-vk-txt-2d.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/job/gx-cr-job-manager.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sampler/gx-vk-smp-manager.hpp"
#include "../sampler/gx-vk-smp-sampler.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-txt-util.hpp"

gearoenix::vulkan::texture::Texture2D::Texture2D(const render::texture::TextureInfo& info, std::string&& in_name)
    : render::texture::Texture2D(std::move(in_name), info)
    , view(new image::View(std::make_shared<image::Image>(name, info.get_width(), info.get_height(), info.get_depth(), convert_image_type(info.get_type()), static_cast<std::uint32_t>(compute_mipmaps_count(info)), 1u,
          convert_image_format(info.get_format()), 0u,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | (render::texture::format_is_depth(info.get_format()) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))))
    , view_index(descriptor::Bindless::get().allocate_2d_image(view->get_vulkan_data()))
    , mips([this] {
        std::vector<std::shared_ptr<image::View>> result;
        const auto mip_count = view->get_image()->get_mipmap_levels();
        result.reserve(mip_count);
        for (std::uint32_t mip = 0; mip < mip_count; ++mip) {
            result.push_back(std::make_shared<image::View>(std::shared_ptr(view->get_image()), mip, 1));
        }
        return result;
    }())
    , sampler_index(sampler::Manager::get().get_sampler(info.get_sampler_info())->get_bindless_index())
{
}

gearoenix::vulkan::texture::Texture2D::~Texture2D() = default;

void gearoenix::vulkan::texture::Texture2D::write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& e, const bool c) const
{
    render::texture::Texture2D::write(s, e, c);
    if (!c) {
        return;
    }

    const auto format = info.get_format();
    const auto pixel_size = render::texture::format_pixel_size(format);
    const auto mips_count = static_cast<std::uint32_t>(get_mipmaps_count());
    const auto& img = view->get_image();
    const auto aspect_flags = img->get_aspect_flags();
    auto level_width = info.get_width();
    auto level_height = info.get_height();

    std::vector<std::shared_ptr<buffer::Buffer>> staging_buffers;
    staging_buffers.reserve(mips_count);
    for (std::uint32_t mip = 0; mip < mips_count; ++mip) {
        staging_buffers.push_back(buffer::Manager::get().create_staging(static_cast<std::int64_t>(pixel_size) * static_cast<std::int64_t>(level_width) * static_cast<std::int64_t>(level_height)));
        level_width = std::max(1u, level_width >> 1);
        level_height = std::max(1u, level_height >> 1);
    }

    auto cmd = command::Manager::create_thread_independent();
    cmd->begin();
    const auto vk_cmd = cmd->get_vulkan_data();

    img->transit(vk_cmd, image::TransitionRequest::transfer_src());

    level_width = info.get_width();
    level_height = info.get_height();
    for (std::uint32_t mip = 0; mip < mips_count; ++mip) {
        VkBufferImageCopy region;
        GX_SET_ZERO(region);
        region.bufferOffset = staging_buffers[mip]->get_offset();
        region.imageSubresource.aspectMask = aspect_flags;
        region.imageSubresource.mipLevel = mip;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageExtent = { level_width, level_height, 1 };

        vkCmdCopyImageToBuffer(vk_cmd, img->get_vulkan_data(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging_buffers[mip]->get_vulkan_data(), 1, &region);

        level_width = std::max(1u, level_width >> 1);
        level_height = std::max(1u, level_height >> 1);
    }

    img->transit(vk_cmd, image::TransitionRequest::shader_read());

    cmd->end();

    auto fence = std::make_shared<sync::Fence>();
    queue::Queue::get().submit(*cmd, *fence);

    const auto width = info.get_width();
    const auto height = info.get_height();
    auto sss = std::make_shared<std::vector<std::shared_ptr<platform::stream::Stream>>>();
    core::job::EndCaller end([e, sss, s]() mutable {
        core::job::send_job_to_pool([e, sss = std::move(sss), s]() mutable {
            for (auto& ss : *sss) {
                s->write(*ss);
            }
            (void)e;
        });
    });

    core::job::send_job_to_pool([fence = std::move(fence), cmd = std::move(cmd), staging_buffers = std::move(staging_buffers), end = std::move(end), sss, width, height, pixel_size, mips_count, format] {
        fence->wait();

        auto level_width = width;
        auto level_height = height;
        for (std::uint32_t mip = 0; mip < mips_count; ++mip) {
            const auto data_size = pixel_size * level_width * level_height;
            const auto* src = static_cast<const std::uint8_t*>(staging_buffers[mip]->get_allocated_memory()->get_data());
            GX_ASSERT_D(src);
            std::vector data(src, src + data_size);

            auto ms = std::make_shared<platform::stream::Memory>();
            sss->push_back(ms);
            write_gx3d_image(std::move(ms), std::move(data), level_width, level_height, format, core::job::EndCaller(end));

            level_width = std::max(1u, level_width >> 1);
            level_height = std::max(1u, level_height >> 1);
        }
    });
}
#endif

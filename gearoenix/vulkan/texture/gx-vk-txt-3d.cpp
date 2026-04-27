#include "gx-vk-txt-3d.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-manager.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../../render/texture/gx-rnd-txt-ktx2.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sampler/gx-vk-smp-manager.hpp"
#include "../sampler/gx-vk-smp-sampler.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-txt-util.hpp"

gearoenix::vulkan::texture::Texture3D::Texture3D(const render::texture::TextureInfo& info, std::string&& in_name)
    : render::texture::Texture3D(std::move(in_name), info)
    , view(new image::View(std::make_shared<image::Image>(
          name,
          info.get_width(),
          info.get_height(),
          info.get_depth(),
          convert_image_type(info.get_type()),
          1u,
          1u,
          convert_image_format(info.get_format()),
          vk::ImageCreateFlags { },
          vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage)))
{
    shader_resource_index = descriptor::Bindless::get().allocate_3d_image(view);
    sampler_shader_resource_index = sampler::Manager::get().get_sampler(info.get_sampler_info())->get_bindless_index();
}

gearoenix::vulkan::texture::Texture3D::~Texture3D() = default;

void gearoenix::vulkan::texture::Texture3D::write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& e, const bool c) const
{
    render::texture::Texture3D::write(s, e, c);
    if (!c) {
        return;
    }
    auto& img = *view->get_image();
    const auto width = info.get_width();
    const auto height = info.get_height();
    const auto depth = info.get_depth();
    const auto format = info.get_format();
    const auto pixel_size = render::texture::format_pixel_size(format);
    const auto total_bytes = static_cast<std::int64_t>(pixel_size) * width * height * depth;
    auto staging = buffer::Manager::create_staging(total_bytes);

    auto cmd = command::Manager::create_thread_independent();
    cmd->begin();
    const auto vk_cmd = cmd->get_vulkan_data();
    img.transit(vk_cmd, image::TransitionRequest::transfer_src());

    vk::BufferImageCopy region;
    region.bufferOffset = staging->get_offset();
    region.imageSubresource.aspectMask = img.get_aspect_flags();
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = vk::Extent3D { width, height, depth };
    vk_cmd.copyImageToBuffer(img.get_vulkan_data(), vk::ImageLayout::eTransferSrcOptimal, staging->get_vulkan_data(), region);

    img.transit(vk_cmd, image::TransitionRequest::shader_read());
    cmd->end();

    auto fence = std::make_shared<sync::Fence>();
    queue::Queue::get().submit(*cmd, *fence);

    core::job::send_job_to_pool([fence = std::move(fence), cmd = std::move(cmd), staging = std::move(staging), s, e, width, height, depth, format, total_bytes] {
        fence->wait();
        const auto* src = static_cast<const std::uint8_t*>(staging->get_allocated_memory()->get_data());
        GX_ASSERT_D(src);
        std::vector<std::uint8_t> pixels(src, src + total_bytes);
        const auto blob = render::texture::encode_ktx2_3d(pixels.data(), width, height, depth, format);
        s->write(blob);
        (void)e;
    });
}
#endif

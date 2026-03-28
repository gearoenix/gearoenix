#include "gx-vk-txt-util.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-manager.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"
#include "../../render/texture/gx-rnd-txt-texture.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"

vk::ImageType gearoenix::vulkan::texture::convert_image_type(const render::texture::Type type)
{
    switch (type) {
    case render::texture::Type::Texture2D:
        return vk::ImageType::e2D;
    case render::texture::Type::Texture3D:
        return vk::ImageType::e3D;
    case render::texture::Type::TextureCube:
        return vk::ImageType::e2D;
    default:
        GX_UNEXPECTED;
    }
}

vk::Format gearoenix::vulkan::texture::convert_image_format(const render::texture::TextureFormat format)
{
    switch (format) {
    case render::texture::TextureFormat::RgbaUint8:
        return vk::Format::eR8G8B8A8Unorm;
    case render::texture::TextureFormat::RgbaFloat32:
        return vk::Format::eR32G32B32A32Sfloat;
    case render::texture::TextureFormat::RgbFloat32:
        return vk::Format::eR32G32B32Sfloat;
    case render::texture::TextureFormat::RgFloat32:
        return vk::Format::eR32G32Sfloat;
    case render::texture::TextureFormat::Float32:
        return vk::Format::eR32Sfloat;
    case render::texture::TextureFormat::RgbaFloat16:
        return vk::Format::eR16G16B16A16Sfloat;
    case render::texture::TextureFormat::RgbFloat16:
        return vk::Format::eR16G16B16Sfloat;
    case render::texture::TextureFormat::RgFloat16:
        return vk::Format::eR16G16Sfloat;
    case render::texture::TextureFormat::Float16:
        return vk::Format::eR16Sfloat;
    case render::texture::TextureFormat::D32:
        return vk::Format::eD32Sfloat;
    case render::texture::TextureFormat::D24:
        return vk::Format::eX8D24UnormPack32;
    case render::texture::TextureFormat::D16:
        return vk::Format::eD16Unorm;
    default:
        GX_UNEXPECTED;
    }
}

bool gearoenix::vulkan::texture::has_depth(const vk::Format format)
{
    switch (format) {
    case vk::Format::eD32Sfloat:
    case vk::Format::eX8D24UnormPack32:
    case vk::Format::eD16Unorm:
        return true;
    default:
        return false;
    }
}

void gearoenix::vulkan::texture::write_gpu_texture_data(
    image::Image& img,
    const render::texture::TextureInfo& info,
    const std::uint32_t mips_count,
    const std::uint32_t layer_count,
    const std::shared_ptr<platform::stream::Stream>& s,
    const core::job::EndCaller<>& e)
{
    const auto format = info.get_format();
    const auto pixel_size = render::texture::format_pixel_size(format);
    const auto aspect_flags = img.get_aspect_flags();
    const auto width = info.get_width();
    const auto height = info.get_height();
    const auto alignment = static_cast<std::int64_t>(device::Physical::get().get_properties().limits.optimalBufferCopyRowPitchAlignment);

    std::vector<std::shared_ptr<buffer::Buffer>> staging_buffers;
    staging_buffers.reserve(layer_count * mips_count);
    for (std::uint32_t layer = 0; layer < layer_count; ++layer) {
        auto level_width = width;
        auto level_height = height;
        for (std::uint32_t mip = 0; mip < mips_count; ++mip) {
            staging_buffers.push_back(buffer::Manager::get().create_staging(
                static_cast<std::int64_t>(pixel_size) * static_cast<std::int64_t>(level_width) * static_cast<std::int64_t>(level_height), alignment));
            level_width = std::max(1u, level_width >> 1);
            level_height = std::max(1u, level_height >> 1);
        }
    }

    auto cmd = command::Manager::create_thread_independent();
    cmd->begin();
    const auto vk_cmd = cmd->get_vulkan_data();

    img.transit(vk_cmd, image::TransitionRequest::transfer_src());

    for (std::uint32_t layer = 0; layer < layer_count; ++layer) {
        auto level_width = width;
        auto level_height = height;
        for (std::uint32_t mip = 0; mip < mips_count; ++mip) {
            const auto buffer_index = layer * mips_count + mip;
            vk::BufferImageCopy region;
            region.bufferOffset = staging_buffers[buffer_index]->get_offset();
            region.imageSubresource.aspectMask = aspect_flags;
            region.imageSubresource.mipLevel = mip;
            region.imageSubresource.baseArrayLayer = layer;
            region.imageSubresource.layerCount = 1;
            region.imageExtent = vk::Extent3D { level_width, level_height, 1 };

            vk_cmd.copyImageToBuffer(img.get_vulkan_data(), vk::ImageLayout::eTransferSrcOptimal, staging_buffers[buffer_index]->get_vulkan_data(), region);

            level_width = std::max(1u, level_width >> 1);
            level_height = std::max(1u, level_height >> 1);
        }
    }

    img.transit(vk_cmd, image::TransitionRequest::shader_read());

    cmd->end();

    auto fence = std::make_shared<sync::Fence>();
    queue::Queue::get().submit(*cmd, *fence);

    auto sss = std::make_shared<std::vector<std::shared_ptr<platform::stream::Stream>>>();
    core::job::EndCaller end([e, sss, s]() mutable {
        core::job::send_job_to_pool([e, sss = std::move(sss), s]() mutable {
            for (auto& ss : *sss) {
                s->write(*ss);
            }
            (void)e;
        });
    });

    core::job::send_job_to_pool([fence = std::move(fence), cmd = std::move(cmd), staging_buffers = std::move(staging_buffers), end = std::move(end), sss, width, height, pixel_size, mips_count, layer_count, format] {
        fence->wait();

        for (std::uint32_t layer = 0; layer < layer_count; ++layer) {
            auto level_width = width;
            auto level_height = height;
            for (std::uint32_t mip = 0; mip < mips_count; ++mip) {
                const auto buffer_index = layer * mips_count + mip;
                const auto data_size = pixel_size * level_width * level_height;
                const auto* src = static_cast<const std::uint8_t*>(staging_buffers[buffer_index]->get_allocated_memory()->get_data());
                GX_ASSERT_D(src);
                std::vector data(src, src + data_size);

                auto ms = std::make_shared<platform::stream::Memory>();
                sss->push_back(ms);
                render::texture::Texture::write_gx3d_image(std::move(ms), std::move(data), level_width, level_height, format, core::job::EndCaller(end));

                level_width = std::max(1u, level_width >> 1);
                level_height = std::max(1u, level_height >> 1);
            }
        }
    });
}

#endif

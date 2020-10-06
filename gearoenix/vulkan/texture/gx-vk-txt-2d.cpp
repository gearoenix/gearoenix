#include "gx-vk-txt-2d.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-allocator.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-manager.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-memory.hpp"

std::uint32_t gearoenix::vulkan::texture::Texture2D::compute_mipmaps_count(
    const gearoenix::render::texture::TextureInfo& info,
    const std::size_t img_width, const std::size_t img_height) noexcept
{
    if (!info.has_mipmap)
        return 1;
    return math::Numeric::floor_log2(math::Numeric::maximum(img_width, img_height)) + 1;
}

gearoenix::vulkan::texture::Texture2D::Texture2D(
    const core::Id id,
    std::string name,
    engine::Engine* const e,
    const std::vector<std::vector<std::uint8_t>>& data,
    const render::texture::TextureInfo& info,
    const std::size_t img_width,
    const std::size_t img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : render::texture::Texture2D(id, std::move(name), info.format, info.sampler_info, e)
    , view(new image::View(std::make_shared<image::Image>(
          static_cast<std::uint32_t>(img_width), static_cast<std::uint32_t>(img_height), 1u,
          compute_mipmaps_count(info, img_width, img_height), 1u,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          convert(info.format), *e->get_memory_manager())))
{
    const auto sz = view->get_image()->get_allocated_memory()->get_allocator()->get_size();
    auto buff = e->get_vulkan_buffer_manager()->create_upload_buffer(sz);
    std::vector<std::uint8_t> gathered_data(sz);
    std::size_t index = 0;
    for (const auto& d : data) {
        std::memcpy(&gathered_data[index], d.data(), d.size());
        index += d.size();
    }
    std::memcpy(buff->get_allocated_memory()->get_data(), gathered_data.data(), index);
    e->get_image_manager()->upload(view->get_image(), std::move(buff), call);
}

gearoenix::vulkan::texture::Texture2D::~Texture2D() noexcept = default;

void gearoenix::vulkan::texture::Texture2D::write_gx3d(
    const std::shared_ptr<system::stream::Stream>&,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept {
    GX_UNEXPECTED
}

VkFormat gearoenix::vulkan::texture::Texture2D::convert(const render::texture::TextureFormat format) noexcept
{
    switch (format) {
    case render::texture::TextureFormat::RgbaUint8:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case render::texture::TextureFormat::RgbaFloat32:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    default:
        GX_UNEXPECTED
    }
}

#endif

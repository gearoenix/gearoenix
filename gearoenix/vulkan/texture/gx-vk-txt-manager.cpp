#include "gx-vk-txt-manager.hpp"

#include <memory>
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-manager.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "gx-vk-txt-2d.hpp"
#include "gx-vk-txt-cube.hpp"
#include "gx-vk-txt-target.hpp"

namespace {
[[nodiscard]] std::shared_ptr<gearoenix::vulkan::buffer::Buffer> create_staging_buffer(const std::vector<std::uint8_t>& pixels)
{
    GX_ASSERT_D(!pixels.empty());
    auto buff = gearoenix::vulkan::buffer::Manager::get().create_staging(static_cast<std::int64_t>(pixels.size()));
    buff->write(pixels.data(), static_cast<std::int64_t>(pixels.size()));
    return buff;
}
}

gearoenix::vulkan::texture::Manager::Manager()
    : Singleton<Manager>(this)
{
}

gearoenix::vulkan::texture::Manager::~Manager() = default;

void gearoenix::vulkan::texture::Manager::create_2d_from_pixels_v(
    std::string&& name,
    std::vector<std::vector<std::uint8_t>>&& pixels,
    const render::texture::TextureInfo& info,
    core::job::EndCallerShared<render::texture::Texture2D>&& c)
{
    auto result = std::make_shared<Texture2D>(info, std::move(name));
    std::vector<std::vector<std::shared_ptr<buffer::Buffer>>> buffers(1);
    buffers[0].reserve(pixels.size());
    for (auto& mip_pixels : pixels) {
        buffers[0].push_back(create_staging_buffer(mip_pixels));
    }
    auto img = std::shared_ptr(result->get_view()->get_image());
    c.set_return(std::move(result));
    image::Manager::upload(std::move(img), std::move(buffers), info.get_has_mipmap(), core::job::EndCaller([c = std::move(c)] { }));
}

void gearoenix::vulkan::texture::Manager::create_cube_from_pixels_v(
    std::string&& name,
    std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels,
    const render::texture::TextureInfo& info,
    core::job::EndCallerShared<render::texture::TextureCube>&& c)
{
    auto result = std::make_shared<TextureCube>(info, std::move(name));

    std::vector<std::vector<std::shared_ptr<buffer::Buffer>>> buffers;
    buffers.reserve(pixels.size());
    for (const auto& face_pixels : pixels) {
        std::vector<std::shared_ptr<buffer::Buffer>> face_buffers;
        face_buffers.reserve(face_pixels.size());
        for (auto& mip_pixels : face_pixels) {
            face_buffers.push_back(create_staging_buffer(mip_pixels));
        }
        buffers.push_back(std::move(face_buffers));
    }

    auto img = std::shared_ptr(result->get_view()->get_image());
    c.set_return(std::move(result));
    image::Manager::get().upload(std::move(img), std::move(buffers), info.get_has_mipmap(), core::job::EndCaller([c = std::move(c)] { }));
}

void gearoenix::vulkan::texture::Manager::create_target_v(
    std::string&& name,
    std::vector<render::texture::Attachment>&& attachments,
    core::job::EndCallerShared<render::texture::Target>&& c)
{
    Target::construct(std::move(name), std::move(attachments), std::move(c));
}
#endif

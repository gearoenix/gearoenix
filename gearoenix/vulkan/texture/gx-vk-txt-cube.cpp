#include "gx-vk-txt-cube.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-manager.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "gx-vk-txt-2d.hpp"

gearoenix::vulkan::texture::TextureCube::TextureCube(
    const core::Id id,
    std::string name,
    engine::Engine* const eng,
    std::vector<std::vector<std::vector<std::uint8_t>>> data,
    const render::texture::TextureInfo& info,
    const std::uint64_t aspect,
    const core::job::EndCaller<core::job::EndCallerIgnore>& call)
    : render::texture::TextureCube(id, std::move(name), info.format, info.sampler_info, eng)
    , view(new image::View(std::make_shared<image::Image>(
          static_cast<std::uint32_t>(aspect), static_cast<std::uint32_t>(aspect), 1u,
          static_cast<std::uint32_t>(compute_mipmaps_count(aspect, aspect)), 6u,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
          Texture2D::convert(info.format), *eng->get_memory_manager())))
{
    const auto sz = view->get_image()->get_allocated_memory()->get_allocator()->get_size();
    const auto sz_per_face = sz / 6;
    auto buff = eng->get_vulkan_buffer_manager()->create_upload_buffer(sz);
    std::vector<std::uint8_t> gathered_data;
    gathered_data.reserve(sz);
    for (std::uint64_t face_index = 0, ptr = reinterpret_cast<std::uint64_t>(buff->get_allocated_memory()->get_data()); face_index < 6; ++face_index, ptr += sz_per_face) {
        gathered_data.clear();
        for (const auto& d : data[face_index]) {
            gathered_data.insert(gathered_data.end(), d.begin(), d.end());
        }
        std::memcpy(reinterpret_cast<void*>(ptr), gathered_data.data(), gathered_data.size());
    }
    eng->get_image_manager()->upload(view->get_image(), std::move(buff), call);
}

gearoenix::vulkan::texture::TextureCube::~TextureCube() = default;

void gearoenix::vulkan::texture::TextureCube::write_gx3d(
    const std::shared_ptr<platform::stream::Stream>&,
    const core::job::EndCaller<core::job::EndCallerIgnore>&)
{
    GX_UNIMPLEMENTED
}

#endif

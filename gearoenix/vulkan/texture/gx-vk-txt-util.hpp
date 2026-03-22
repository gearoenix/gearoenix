#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../render/texture/gx-rnd-txt-format.hpp"
#include "../../render/texture/gx-rnd-txt-type.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::vulkan::image {
struct Image;
}

namespace gearoenix::render::texture {
struct TextureInfo;
}

namespace gearoenix::vulkan::texture {
[[nodiscard]] vk::ImageType convert_image_type(render::texture::Type type);
[[nodiscard]] vk::Format convert_image_format(render::texture::TextureFormat format);
[[nodiscard]] bool has_depth(vk::Format format);

void write_gpu_texture_data(
    image::Image& img,
    const render::texture::TextureInfo& info,
    std::uint32_t mips_count,
    std::uint32_t layer_count,
    const std::shared_ptr<platform::stream::Stream>& s,
    const core::job::EndCaller<>& e);
}

#endif

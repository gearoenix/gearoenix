#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-format.hpp"
#include "../../render/texture/gx-rnd-txt-type.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::texture {
[[nodiscard]] VkImageType convert_image_type(render::texture::Type type);
[[nodiscard]] VkFormat convert_image_format(render::texture::TextureFormat format);
[[nodiscard]] bool has_depth(VkFormat format);
}

#endif
#pragma once
// Intentionally NOT guarded by GX_RENDER_VULKAN_ENABLED: the numeric VkFormat codes are also part of
// the KTX2 file-format spec, so other modules (e.g. the KTX2 codec) need this mapping even when the
// Vulkan backend is disabled at compile time. The implementation lives in the matching `.cpp` and
// pulls in the Vulkan SDK only behind `GX_RENDER_VULKAN_ENABLED` to assert that our hardcoded numeric
// codes still match `vk::Format` enum values.

#include "../../render/texture/gx-rnd-txt-format.hpp"

#include <cstdint>

namespace gearoenix::vulkan::texture {
/// Map a TextureFormat to its VkFormat numeric code. Returns 0 (VK_FORMAT_UNDEFINED) for unsupported formats.
[[nodiscard]] std::uint32_t format_to_vk_format_code(render::texture::TextureFormat f);

/// Inverse of `format_to_vk_format_code`. Returns TextureFormat::Unknown for unsupported codes.
[[nodiscard]] render::texture::TextureFormat format_from_vk_format_code(std::uint32_t v);
}
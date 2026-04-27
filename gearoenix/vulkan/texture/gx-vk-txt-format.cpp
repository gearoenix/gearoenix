#include "gx-vk-txt-format.hpp"
#include "../../render/gx-rnd-build-configuration.hpp"

#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#endif

namespace {
// Numeric VkFormat codes (Vulkan / KTX2 spec). These are the canonical values; when the Vulkan SDK
// is available, we assert they match the `vk::Format` enum below.
constexpr std::uint32_t vk_format_undefined = 0;
constexpr std::uint32_t vk_format_r8_unorm = 9;
constexpr std::uint32_t vk_format_r8g8b8a8_unorm = 37;
constexpr std::uint32_t vk_format_r16_sfloat = 76;
constexpr std::uint32_t vk_format_r16g16_sfloat = 83;
constexpr std::uint32_t vk_format_r16g16b16_sfloat = 90;
constexpr std::uint32_t vk_format_r16g16b16a16_sfloat = 97;
constexpr std::uint32_t vk_format_r32_sfloat = 100;
constexpr std::uint32_t vk_format_r32g32_sfloat = 103;
constexpr std::uint32_t vk_format_r32g32b32_sfloat = 106;
constexpr std::uint32_t vk_format_r32g32b32a32_sfloat = 109;
constexpr std::uint32_t vk_format_d16_unorm = 124;
constexpr std::uint32_t vk_format_x8_d24_unorm_pack32 = 125;
constexpr std::uint32_t vk_format_d32_sfloat = 126;

#if GX_RENDER_VULKAN_ENABLED
// When vulkan-hpp is in scope, cross-check our hardcoded codes against the SDK's enum.
static_assert(vk_format_undefined == static_cast<std::uint32_t>(vk::Format::eUndefined));
static_assert(vk_format_r8_unorm == static_cast<std::uint32_t>(vk::Format::eR8Unorm));
static_assert(vk_format_r8g8b8a8_unorm == static_cast<std::uint32_t>(vk::Format::eR8G8B8A8Unorm));
static_assert(vk_format_r16_sfloat == static_cast<std::uint32_t>(vk::Format::eR16Sfloat));
static_assert(vk_format_r16g16_sfloat == static_cast<std::uint32_t>(vk::Format::eR16G16Sfloat));
static_assert(vk_format_r16g16b16_sfloat == static_cast<std::uint32_t>(vk::Format::eR16G16B16Sfloat));
static_assert(vk_format_r16g16b16a16_sfloat == static_cast<std::uint32_t>(vk::Format::eR16G16B16A16Sfloat));
static_assert(vk_format_r32_sfloat == static_cast<std::uint32_t>(vk::Format::eR32Sfloat));
static_assert(vk_format_r32g32_sfloat == static_cast<std::uint32_t>(vk::Format::eR32G32Sfloat));
static_assert(vk_format_r32g32b32_sfloat == static_cast<std::uint32_t>(vk::Format::eR32G32B32Sfloat));
static_assert(vk_format_r32g32b32a32_sfloat == static_cast<std::uint32_t>(vk::Format::eR32G32B32A32Sfloat));
static_assert(vk_format_d16_unorm == static_cast<std::uint32_t>(vk::Format::eD16Unorm));
static_assert(vk_format_x8_d24_unorm_pack32 == static_cast<std::uint32_t>(vk::Format::eX8D24UnormPack32));
static_assert(vk_format_d32_sfloat == static_cast<std::uint32_t>(vk::Format::eD32Sfloat));
#endif
}

std::uint32_t gearoenix::vulkan::texture::format_to_vk_format_code(const render::texture::TextureFormat f)
{
    using F = render::texture::TextureFormat;
    switch (f) {
    case F::R8Unorm:
        return vk_format_r8_unorm;
    case F::RgbaUint8:
        return vk_format_r8g8b8a8_unorm;
    case F::Float16:
        return vk_format_r16_sfloat;
    case F::RgFloat16:
        return vk_format_r16g16_sfloat;
    case F::RgbFloat16:
        return vk_format_r16g16b16_sfloat;
    case F::RgbaFloat16:
        return vk_format_r16g16b16a16_sfloat;
    case F::Float32:
        return vk_format_r32_sfloat;
    case F::RgFloat32:
        return vk_format_r32g32_sfloat;
    case F::RgbFloat32:
        return vk_format_r32g32b32_sfloat;
    case F::RgbaFloat32:
        return vk_format_r32g32b32a32_sfloat;
    case F::D16:
        return vk_format_d16_unorm;
    case F::D24:
        return vk_format_x8_d24_unorm_pack32;
    case F::D32:
        return vk_format_d32_sfloat;
    default:
        return vk_format_undefined;
    }
}

gearoenix::render::texture::TextureFormat gearoenix::vulkan::texture::format_from_vk_format_code(const std::uint32_t v)
{
    using F = render::texture::TextureFormat;
    switch (v) {
    case vk_format_r8_unorm:
        return F::R8Unorm;
    case vk_format_r8g8b8a8_unorm:
        return F::RgbaUint8;
    case vk_format_r16_sfloat:
        return F::Float16;
    case vk_format_r16g16_sfloat:
        return F::RgFloat16;
    case vk_format_r16g16b16_sfloat:
        return F::RgbFloat16;
    case vk_format_r16g16b16a16_sfloat:
        return F::RgbaFloat16;
    case vk_format_r32_sfloat:
        return F::Float32;
    case vk_format_r32g32_sfloat:
        return F::RgFloat32;
    case vk_format_r32g32b32_sfloat:
        return F::RgbFloat32;
    case vk_format_r32g32b32a32_sfloat:
        return F::RgbaFloat32;
    case vk_format_d16_unorm:
        return F::D16;
    case vk_format_x8_d24_unorm_pack32:
        return F::D24;
    case vk_format_d32_sfloat:
        return F::D32;
    default:
        return F::Unknown;
    }
}
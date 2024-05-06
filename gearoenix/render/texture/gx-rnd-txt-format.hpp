#ifndef GEAROENIX_RENDER_TEXTURE_FORMAT_HPP
#define GEAROENIX_RENDER_TEXTURE_FORMAT_HPP
#include "../../core/gx-cr-types.hpp"
#include <cstdlib>
#include <string>

namespace gearoenix::render::texture {
enum struct TextureFormat : core::TypeId {
    RgbaFloat16 = 1,
    RgbFloat16 = 2,
    RgFloat16 = 3,
    Float16 = 4,

    RgbaFloat24 = 5,
    RgbFloat24 = 6,
    RgFloat24 = 7,
    Float24 = 8,

    RgbaFloat32 = 9,
    RgbFloat32 = 10,
    RgFloat32 = 11,
    Float32 = 12,

    RgbaUint8 = 13,

    D16 = 14,
    D24 = 15,
    D32 = 16,

    Unknown = 255,
};

[[nodiscard]] constexpr std::size_t format_component_bits_count(const TextureFormat f)
{
    switch (f) {
    case TextureFormat::RgbaFloat16:
    case TextureFormat::RgbFloat16:
    case TextureFormat::RgFloat16:
    case TextureFormat::Float16:
    case TextureFormat::D16:
        return 16;
    case TextureFormat::RgbaFloat24:
    case TextureFormat::RgbFloat24:
    case TextureFormat::RgFloat24:
    case TextureFormat::Float24:
    case TextureFormat::D24:
        return 24;
    case TextureFormat::RgbaFloat32:
    case TextureFormat::RgbFloat32:
    case TextureFormat::RgFloat32:
    case TextureFormat::Float32:
    case TextureFormat::D32:
        return 32;
    case TextureFormat::RgbaUint8:
        return 8;
    default:
        return static_cast<std::size_t>(-1);
    }
}

[[nodiscard]] constexpr bool format_has_float_component(const TextureFormat f)
{
    switch (f) {
    case TextureFormat::RgbaFloat16:
    case TextureFormat::RgbFloat16:
    case TextureFormat::RgFloat16:
    case TextureFormat::Float16:
    case TextureFormat::RgbaFloat24:
    case TextureFormat::RgbFloat24:
    case TextureFormat::RgFloat24:
    case TextureFormat::Float24:
    case TextureFormat::RgbaFloat32:
    case TextureFormat::RgbFloat32:
    case TextureFormat::RgFloat32:
    case TextureFormat::Float32:
    case TextureFormat::D16:
    case TextureFormat::D24:
    case TextureFormat::D32:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] constexpr std::size_t format_components_count(const TextureFormat f)
{
    switch (f) {
    case TextureFormat::RgbaFloat16:
    case TextureFormat::RgbaFloat24:
    case TextureFormat::RgbaFloat32:
    case TextureFormat::RgbaUint8:
        return 4;
    case TextureFormat::RgbFloat16:
    case TextureFormat::RgbFloat24:
    case TextureFormat::RgbFloat32:
        return 3;
    case TextureFormat::RgFloat16:
    case TextureFormat::RgFloat24:
    case TextureFormat::RgFloat32:
        return 2;
    case TextureFormat::Float16:
    case TextureFormat::Float24:
    case TextureFormat::Float32:
    case TextureFormat::D16:
    case TextureFormat::D24:
    case TextureFormat::D32:
        return 1;
    default:
        return 0;
    }
}

[[nodiscard]] constexpr bool format_is_depth(const TextureFormat f)
{
    switch (f) {
    case TextureFormat::D16:
    case TextureFormat::D24:
    case TextureFormat::D32:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] constexpr std::size_t format_pixel_size(const TextureFormat f)
{
    switch (f) {
    case TextureFormat::Float16:
    case TextureFormat::D16:
        return 2;
    case TextureFormat::Float24:
    case TextureFormat::D24:
        return 3;
    case TextureFormat::RgbaUint8:
    case TextureFormat::RgFloat16:
    case TextureFormat::Float32:
    case TextureFormat::D32:
        return 4;
    case TextureFormat::RgbFloat16:
    case TextureFormat::RgFloat24:
        return 6;
    case TextureFormat::RgbaFloat16:
    case TextureFormat::RgFloat32:
        return 8;
    case TextureFormat::RgbFloat24:
        return 9;
    case TextureFormat::RgbaFloat24:
    case TextureFormat::RgbFloat32:
        return 12;
    case TextureFormat::RgbaFloat32:
        return 16;
    default:
        return static_cast<std::size_t>(-1);
    }
}
}

namespace std {
[[nodiscard]] std::string to_string(gearoenix::render::texture::TextureFormat f);
}
#endif

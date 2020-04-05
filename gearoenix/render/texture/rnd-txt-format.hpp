#ifndef GEAROENIX_RENDER_TEXTURE_FORMAT_HPP
#define GEAROENIX_RENDER_TEXTURE_FORMAT_HPP
#include "../../core/cr-types.hpp"
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
    RgbUint8 = 14,
    RgUint8 = 15,
    Uint8 = 16,

    D16 = 17,
    D24 = 18,
    D32 = 19,
};

constexpr bool format_has_float_component(const TextureFormat f) noexcept
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
    case TextureFormat::RgbaUint8:
    case TextureFormat::RgbUint8:
    case TextureFormat::RgUint8:
    case TextureFormat::Uint8:
        return false;
    }
}
}
#endif

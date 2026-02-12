#include "gx-vk-txt-util.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"

VkImageType gearoenix::vulkan::texture::convert_image_type(const render::texture::Type type)
{
    switch (type) {
    case render::texture::Type::Texture2D:
        return VK_IMAGE_TYPE_2D;
    case render::texture::Type::Texture3D:
        return VK_IMAGE_TYPE_3D;
    case render::texture::Type::TextureCube:
        return VK_IMAGE_TYPE_2D;
    default:
        GX_UNEXPECTED;
    }
}

VkFormat gearoenix::vulkan::texture::convert_image_format(const render::texture::TextureFormat format)
{
    switch (format) {
    case render::texture::TextureFormat::RgbaUint8:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case render::texture::TextureFormat::RgbaFloat32:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case render::texture::TextureFormat::RgbFloat32:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case render::texture::TextureFormat::RgFloat32:
        return VK_FORMAT_R32G32_SFLOAT;
    case render::texture::TextureFormat::Float32:
        return VK_FORMAT_R32_SFLOAT;
    case render::texture::TextureFormat::RgbaFloat16:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case render::texture::TextureFormat::RgbFloat16:
        return VK_FORMAT_R16G16B16_SFLOAT;
    case render::texture::TextureFormat::RgFloat16:
        return VK_FORMAT_R16G16_SFLOAT;
    case render::texture::TextureFormat::Float16:
        return VK_FORMAT_R16_SFLOAT;
    case render::texture::TextureFormat::D32:
        return VK_FORMAT_D32_SFLOAT;
    case render::texture::TextureFormat::D24:
        return VK_FORMAT_X8_D24_UNORM_PACK32;
    case render::texture::TextureFormat::D16:
        return VK_FORMAT_D16_UNORM;
    default:
        GX_UNEXPECTED;
    }
}

bool gearoenix::vulkan::texture::has_depth(const VkFormat format)
{
    switch (format) {
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
    case VK_FORMAT_D16_UNORM:
        return true;
    default:
        return false;
    }
}

#endif

#include "gx-rnd-txt-ktx2.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../../vulkan/texture/gx-vk-txt-format.hpp"

#include <ktx.h>
#include <zstd.h>

#include <cstdlib>
#include <cstring>

std::vector<std::uint8_t> gearoenix::render::texture::encode_ktx2_3d(
    const std::uint8_t* const pixels,
    const std::uint32_t width,
    const std::uint32_t height,
    const std::uint32_t depth,
    const TextureFormat format)
{
    const auto vk_format = vulkan::texture::format_to_vk_format_code(format);
    GX_ASSERT_D(0u != vk_format);

    ktxTextureCreateInfo info { };
    info.vkFormat = vk_format;
    info.baseWidth = width;
    info.baseHeight = height;
    info.baseDepth = depth;
    info.numDimensions = 3;
    info.numLevels = 1;
    info.numLayers = 1;
    info.numFaces = 1;
    info.isArray = KTX_FALSE;
    info.generateMipmaps = KTX_FALSE;

    ktxTexture2* texture = nullptr;
    if (KTX_SUCCESS != ktxTexture2_Create(&info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture)) {
        GX_LOG_F("ktxTexture2_Create failed.");
    }

    // libktx wants one z-slice per `SetImageFromMemory` call for 3D textures (the `faceSlice`
    // parameter is the depth-slice index, and `srcSize` must equal a single slice's bytes, not the
    // whole volume). Our `pixels` buffer is depth-major (R fastest, then G, then B = depth), so the
    // slices are contiguous in memory, and we can hand off pointers without any copy.
    const auto pixel_size = format_pixel_size(format);
    const auto slice_bytes = static_cast<std::size_t>(width) * height * pixel_size;
    for (std::uint32_t z = 0; z < depth; ++z) {
        const auto* const slice_ptr = pixels + z * slice_bytes;
        if (KTX_SUCCESS != ktxTexture_SetImageFromMemory(ktxTexture(texture), 0, 0, z, slice_ptr, slice_bytes)) {
            ktxTexture_Destroy(ktxTexture(texture));
            GX_LOG_F("ktxTexture_SetImageFromMemory failed at z-slice " << z);
        }
    }

    // Always Zstd-supercompress at the library's maximum level. >=20 is "ultra" mode — slower and
    // more memory at compress time, but cache writes happen once and the engine prefers smallness.
    const auto max_zstd_level = static_cast<ktx_uint32_t>(ZSTD_maxCLevel());
    if (KTX_SUCCESS != ktxTexture2_DeflateZstd(texture, max_zstd_level)) {
        ktxTexture_Destroy(ktxTexture(texture));
        GX_LOG_F("ktxTexture2_DeflateZstd failed.");
    }

    ktx_uint8_t* out_bytes = nullptr;
    ktx_size_t out_size = 0;
    if (KTX_SUCCESS != ktxTexture_WriteToMemory(ktxTexture(texture), &out_bytes, &out_size)) {
        ktxTexture_Destroy(ktxTexture(texture));
        GX_LOG_F("ktxTexture_WriteToMemory failed.");
    }

    std::vector result(out_bytes, out_bytes + out_size);
    free(out_bytes);
    ktxTexture_Destroy(ktxTexture(texture));
    return result;
}

gearoenix::render::texture::DecodedKtx2 gearoenix::render::texture::decode_ktx2_3d(const std::uint8_t* const bytes, const std::size_t size)
{
    ktxTexture2* texture = nullptr;
    if (KTX_SUCCESS != ktxTexture2_CreateFromMemory(bytes, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture)) {
        GX_LOG_F("ktxTexture2_CreateFromMemory failed.");
    }

    if (ktxTexture2_NeedsTranscoding(texture)) {
        ktxTexture_Destroy(ktxTexture(texture));
        GX_LOG_F("KTX2 input requires transcoding (Basis); only uncompressed / Zstd-supercompressed payloads are supported.");
    }

    DecodedKtx2 result;
    result.width = texture->baseWidth;
    result.height = texture->baseHeight;
    result.depth = texture->baseDepth;
    result.format = vulkan::texture::format_from_vk_format_code(texture->vkFormat);

    if (TextureFormat::Unknown == result.format) {
        ktxTexture_Destroy(ktxTexture(texture));
        GX_LOG_F("KTX2 input uses an unsupported VkFormat.");
    }

    const auto data_size = ktxTexture_GetDataSize(ktxTexture(texture));
    const auto* const data = ktxTexture_GetData(ktxTexture(texture));
    result.pixels.resize(data_size);
    std::memcpy(result.pixels.data(), data, data_size);

    ktxTexture_Destroy(ktxTexture(texture));
    return result;
}

#include "gx-rnd-txt-image.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
// This is a workaround for warnings in stb
#include "../../platform/macro/gx-plt-mcr-disable-warnings.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
// Returning to our warning level
#include "../../platform/macro/gx-plt-mcr-enable-warnings.hpp"

#include <cstring>

void gearoenix::render::texture::Image::encode_write_func(void* const context, void* const data, const int size)
{
    auto* const file = static_cast<platform::stream::Stream*>(context);
    (void)file->write(data, size);
}

void gearoenix::render::texture::Image::decode(
    platform::stream::Stream* const file,
    std::vector<unsigned char>& encoded_data,
    std::uint32_t& img_width,
    std::uint32_t& img_height)
{
    std::vector<unsigned char> img;
    file->read(img);
    std::uint32_t img_channels = 0;
    decode(img.data(), img.size(), 4, encoded_data, img_width, img_height, img_channels);
}

void gearoenix::render::texture::Image::decode(
    const unsigned char* const formatted_data,
    const std::size_t formatted_size,
    const std::optional<std::size_t> requested_channels,
    std::vector<unsigned char>& decoded_data,
    std::uint32_t& img_width,
    std::uint32_t& img_height,
    std::uint32_t& img_channels)
{
    int iw = 0, ih = 0, chs = 0;
    unsigned char* const dd = stbi_load_from_memory(formatted_data, static_cast<int>(formatted_size), &iw, &ih, &chs,
        static_cast<int>(requested_channels.has_value() ? requested_channels.value() : 0));
    if (dd == nullptr) {
        GX_LOG_F("Image decoder error.");
    }
    img_width = static_cast<std::uint32_t>(iw);
    img_height = static_cast<std::uint32_t>(ih);
    img_channels = static_cast<std::uint32_t>(chs);
    chs = requested_channels.has_value() ? static_cast<int>(requested_channels.value()) : chs;
    const auto img_size = static_cast<unsigned int>(img_width * img_height * chs);
    decoded_data.resize(img_size);
    std::memcpy(&(decoded_data[0]), dd, img_size);
    stbi_image_free(dd);
}

void gearoenix::render::texture::Image::decode(
    const unsigned char* const formatted_data,
    const std::size_t formatted_size,
    const std::optional<std::size_t> requested_channels,
    std::vector<float>& decoded_data,
    std::uint32_t& img_width,
    std::uint32_t& img_height,
    std::uint32_t& img_channels)
{
    int iw = 0, ih = 0, chs = 0;
    float* const dd = stbi_loadf_from_memory(formatted_data, static_cast<int>(formatted_size), &iw, &ih, &chs,
        static_cast<int>(requested_channels.has_value() ? requested_channels.value() : 0));
    if (dd == nullptr) {
        GX_LOG_F("Image decoder error.");
    }
    img_width = static_cast<std::size_t>(iw);
    img_height = static_cast<std::size_t>(ih);
    img_channels = static_cast<std::size_t>(chs);
    decoded_data.resize(img_width * img_height * (requested_channels.has_value() ? requested_channels.value() : img_channels));
    std::memcpy(&(decoded_data[0]), dd, decoded_data.size() * sizeof(float));
    stbi_image_free(dd);
}

void gearoenix::render::texture::Image::encode_png(
    platform::stream::Stream& file,
    const std::uint8_t* const data,
    const std::uint32_t img_width,
    const std::uint32_t img_height,
    const std::uint32_t components_count)
{
    stbi_write_png_compression_level = 100;
    stbi_write_png_to_func(
        encode_write_func,
        &file,
        static_cast<int>(img_width),
        static_cast<int>(img_height),
        static_cast<int>(components_count),
        data,
        static_cast<int>(img_width * components_count));
}

void gearoenix::render::texture::Image::encode_hdr(
    platform::stream::Stream& file,
    const void* const data,
    const std::uint32_t img_width,
    const std::uint32_t img_height,
    const std::uint32_t components_count)
{
    stbi_write_hdr_to_func(
        encode_write_func,
        &file,
        static_cast<int>(img_width),
        static_cast<int>(img_height),
        static_cast<int>(components_count),
        static_cast<const float*>(data));
}

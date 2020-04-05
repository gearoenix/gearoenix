#include "rnd-txt-image.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include <cstring>
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include <stb_image_write.h>

void gearoenix::render::texture::Image::encode_write_func(void* const context, void* const data, const int size) noexcept
{
    auto* const file = reinterpret_cast<system::stream::Stream*>(context);
    (void)file->write(data, size);
}

void gearoenix::render::texture::Image::decode(
    system::stream::Stream* const file,
    std::vector<unsigned char>& data,
    std::size_t& img_width,
    std::size_t& img_height) noexcept
{
    std::vector<unsigned char> img;
    file->read(img);
    std::size_t img_channels = 0;
    decode(img.data(), img.size(), 4, data, img_width, img_height, img_channels);
}

void gearoenix::render::texture::Image::decode(
    const unsigned char* const data,
    const std::size_t size,
    const std::optional<std::size_t> requested_channels,
    std::vector<unsigned char>& decoded_data,
    std::size_t& img_width,
    std::size_t& img_height,
    std::size_t& img_channels) noexcept
{
    int iw, ih, chs;
    unsigned char* dd = stbi_load_from_memory(data, static_cast<int>(size), &iw, &ih, &chs,
        static_cast<int>(requested_channels.has_value() ? requested_channels.value() : 0));
    if (dd == nullptr) {
        GXLOGF("Image decoder error.")
    }
    img_width = static_cast<unsigned int>(iw);
    img_height = static_cast<unsigned int>(ih);
    img_channels = static_cast<unsigned int>(chs);
    const unsigned int img_size = img_width * img_height * chs;
    decoded_data.resize(img_size);
    std::memcpy(&(decoded_data[0]), dd, img_size);
    stbi_image_free(dd);
}

void gearoenix::render::texture::Image::decode(
    const unsigned char* const data,
    const std::size_t size,
    const std::optional<std::size_t> requested_channels,
    std::vector<float>& decoded_data,
    std::size_t& img_width,
    std::size_t& img_height,
    std::size_t& img_channels) noexcept
{
    int iw, ih, chs;
    float* const dd = stbi_loadf_from_memory(data, static_cast<int>(size), &iw, &ih, &chs,
        static_cast<int>(requested_channels.has_value() ? requested_channels.value() : 0));
    if (dd == nullptr) {
        GXLOGF("Image decoder error.")
    }
    img_width = static_cast<std::size_t>(iw);
    img_height = static_cast<std::size_t>(ih);
    img_channels = static_cast<std::size_t>(chs);
    decoded_data.resize(img_width * img_height * (requested_channels.has_value() ? requested_channels.value() : img_channels));
    std::memcpy(&(decoded_data[0]), dd, decoded_data.size() * sizeof(float));
    stbi_image_free(dd);
}

void gearoenix::render::texture::Image::encode_png(
    system::stream::Stream* const file,
    const unsigned char* const data,
    const std::size_t img_width,
    const std::size_t img_height,
    const std::size_t components_count) noexcept
{
    stbi_write_png_to_func(encode_write_func, file, img_width, img_height, components_count, data, img_width * components_count);
}

void gearoenix::render::texture::Image::encode_hdr(
    system::stream::Stream* const file,
    const float* const data,
    const std::size_t img_width,
    const std::size_t img_height,
    const std::size_t components_count) noexcept
{
    stbi_write_hdr_to_func(encode_write_func, file, img_width, img_height, components_count, data);
}

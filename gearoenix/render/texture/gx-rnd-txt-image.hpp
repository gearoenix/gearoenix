#ifndef GEAROENIX_RENDER_TEXTURE_IMAGE_HPP
#define GEAROENIX_RENDER_TEXTURE_IMAGE_HPP
#include <cstdint>
#include <optional>
#include <vector>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct Image {

    static void encode_write_func(void* context, void* data, int size);

public:
    Image() = delete;
    ~Image() = delete;

    static void decode(
        platform::stream::Stream* file,
        std::vector<unsigned char>& encoded_data,
        std::uint32_t& img_width,
        std::uint32_t& img_height);
    static void decode(
        const unsigned char* formatted_data,
        std::size_t formatted_size,
        std::optional<std::size_t> requested_channels,
        std::vector<unsigned char>& decoded_data,
        std::uint32_t& img_width,
        std::uint32_t& img_height,
        std::uint32_t& img_channels);
    static void decode(
        const unsigned char* formatted_data,
        std::size_t formatted_size,
        std::optional<std::size_t> requested_channels,
        std::vector<float>& decoded_data,
        std::uint32_t& img_width,
        std::uint32_t& img_height,
        std::uint32_t& img_channels);
    static void encode_png(
        platform::stream::Stream& file,
        const std::uint8_t* data,
        std::uint32_t img_width,
        std::uint32_t img_height,
        std::uint32_t components_count);
    static void encode_hdr(
        platform::stream::Stream& file,
        const void* data,
        std::uint32_t img_width,
        std::uint32_t img_height,
        std::uint32_t components_count);
};
}
#endif

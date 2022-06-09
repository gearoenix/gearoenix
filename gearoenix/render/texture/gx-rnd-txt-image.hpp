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

    static void encode_write_func(void* context, void* data, int size) noexcept;

public:
    Image() = delete;
    ~Image() = delete;

    static void decode(
        platform::stream::Stream* file,
        std::vector<unsigned char>& decoded_data,
        std::size_t& img_width,
        std::size_t& img_height) noexcept;
    static void decode(
        const unsigned char* formatted_data,
        std::size_t formatted_size,
        std::optional<std::size_t> requested_channels,
        std::vector<unsigned char>& decoded_data,
        std::size_t& img_width,
        std::size_t& img_height,
        std::size_t& img_channels) noexcept;
    static void decode(
        const unsigned char* formatted_data,
        std::size_t formatted_size,
        std::optional<std::size_t> requested_channels,
        std::vector<float>& decoded_data,
        std::size_t& img_width,
        std::size_t& img_height,
        std::size_t& img_channels) noexcept;
    static void encode_png(
        platform::stream::Stream& file,
        const std::uint8_t* data,
        std::size_t img_width,
        std::size_t img_height,
        std::size_t components_count) noexcept;
    static void encode_hdr(
        platform::stream::Stream& file,
        const void* data,
        std::size_t img_width,
        std::size_t img_height,
        std::size_t components_count) noexcept;
};
}
#endif

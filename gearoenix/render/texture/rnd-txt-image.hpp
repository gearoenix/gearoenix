#ifndef GEAROENIX_RENDER_TEXTURE_IMAGE_HPP
#define GEAROENIX_RENDER_TEXTURE_IMAGE_HPP
#include <cstdint>
#include <optional>
#include <vector>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::texture {
class Image {
public:
    Image() = delete;
    ~Image() = delete;

    static void decode(
        system::stream::Stream* file,
        std::vector<unsigned char>& decoded_data,
        unsigned int& img_width,
        unsigned int& img_height) noexcept;
    static void decode(
        const unsigned char* formatted_data,
        std::size_t formatted_size,
        unsigned int src_channels,
        std::vector<unsigned char>& decoded_data,
        unsigned int& img_width,
        unsigned int& img_height,
        unsigned int& img_channels) noexcept;
    static void decode(
        const unsigned char* formatted_data,
        std::size_t formatted_size,
        std::optional<std::size_t> requested_channels,
        std::vector<float>& decoded_data,
        std::size_t& img_width,
        std::size_t& img_height,
        std::size_t& img_channels) noexcept;
};
}
#endif
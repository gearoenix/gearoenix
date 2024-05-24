#include "gx-rnd-txt-texture.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-txt-image.hpp"
#include "gx-rnd-txt-pixel-iterator.hpp"

gearoenix::render::texture::Texture::Texture(
    std::string name,
    const TextureInfo& info,
    engine::Engine& e)
    : e(e)
    , name(std::move(name))
    , info(info)
{
}

void gearoenix::render::texture::Texture::write_image(
    platform::stream::Stream& s,
    const std::uint8_t* const data,
    const std::uint32_t img_width,
    const std::uint32_t img_height,
    const TextureFormat format)
{
    if (format_has_float_component(format)) {
        if (32 != format_component_bits_count(format)) {
            GX_LOG_F("We can only export 32bit float format.");
        }
        Image::encode_hdr(s, data, img_width, img_height, format_components_count(format));
    } else {
        if (8 != format_component_bits_count(format)) {
            GX_UNIMPLEMENTED;
        }
        Image::encode_png(s, data, img_width, img_height, format_components_count(format));
    }
}

void gearoenix::render::texture::Texture::write_gx3d_image(
    platform::stream::Stream& s,
    const std::uint8_t* const data,
    const std::uint32_t img_width,
    const std::uint32_t img_height,
    const TextureFormat format)
{
    const auto offset_of_size = s.tell();
    s.write_fail_debug(static_cast<std::uint32_t>(0));
    write_image(s, data, img_width, img_height, format);
    const auto curr_off = s.tell();
    s.seek(offset_of_size);
    const auto sz = static_cast<std::uint32_t>((curr_off - offset_of_size) - sizeof(std::uint32_t));
    GX_ASSERT(sz != 0);
    s.write_fail_debug(sz);
    s.seek(curr_off);
}

gearoenix::render::texture::Texture::~Texture() = default;

void gearoenix::render::texture::Texture::write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>&) const
{
    s->write_fail_debug(name);
    info.write(*s);
}

std::size_t gearoenix::render::texture::Texture::get_mipmaps_count() const
{
    return compute_mipmaps_count(info);
}

std::vector<std::uint8_t> gearoenix::render::texture::Texture::convert_pixels(
    const float* const data,
    const std::size_t in_components_count,
    const std::size_t pixels_count,
    const std::size_t out_components_count)
{
    auto iter = Pixel<float>::ConstIterator(data, in_components_count, pixels_count);
    const auto end = iter + (pixels_count + 1);
    std::vector<std::uint8_t> result(out_components_count * pixels_count);
    for (; iter != end; ++iter) {
        std::size_t i = 0;
        const std::size_t index = iter.get_pixel_index() * out_components_count;
        const float* const pixel_elements = iter.get_data();
        for (; i < in_components_count && i < out_components_count; ++i) {
            const int c = static_cast<int>(pixel_elements[i] * 255.0f + 0.501f);
            result[index + i] = c >= 255 ? 255 : c <= 0.0 ? 0
                                                          : static_cast<std::uint8_t>(c);
        }
        for (; i < out_components_count; ++i) {
            result[index + i] = i < 4 ? 0 : 255;
        }
    }
    return result;
}

std::vector<std::vector<std::uint8_t>> gearoenix::render::texture::Texture::convert_float_pixels(
    const std::vector<std::vector<std::uint8_t>>& data,
    const std::size_t in_components_count,
    const std::size_t out_components_count)
{
    std::vector<std::vector<std::uint8_t>> result;
    result.reserve(data.size());
    const auto pixel_size = sizeof(float) * in_components_count;
    for (auto& d : data)
        result.push_back(convert_pixels(
            reinterpret_cast<const float*>(d.data()),
            in_components_count,
            d.size() / pixel_size,
            out_components_count));
    return result;
}

std::vector<std::vector<std::vector<std::uint8_t>>> gearoenix::render::texture::Texture::convert_float_pixels(
    const std::vector<std::vector<std::vector<std::uint8_t>>>& data,
    const std::size_t in_components_count,
    const std::size_t out_components_count)
{
    std::vector<std::vector<std::vector<std::uint8_t>>> result;
    result.reserve(data.size());
    for (auto& d : data)
        result.push_back(convert_float_pixels(d, in_components_count, out_components_count));
    return result;
}

std::size_t gearoenix::render::texture::Texture::compute_mipmaps_count(
    const std::size_t img_width, const std::size_t img_height)
{
    return math::Numeric::floor_log2(img_width > img_height ? img_width : img_height) + 1;
}

std::size_t gearoenix::render::texture::Texture::compute_mipmaps_count(const TextureInfo& info)
{
    return info.get_has_mipmap() ? compute_mipmaps_count(info.get_width(), info.get_height()) : 1;
}

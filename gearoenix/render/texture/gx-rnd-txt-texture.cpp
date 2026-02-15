#include "gx-rnd-txt-texture.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-txt-image.hpp"
#include "gx-rnd-txt-pixel-iterator.hpp"

gearoenix::render::texture::Texture::Texture(std::string name, const TextureInfo& info)
    : name(std::move(name))
    , info(info)
{
}

void gearoenix::render::texture::Texture::write_image(platform::stream::Stream& s, const std::uint8_t* const data, const std::uint32_t img_width, const std::uint32_t img_height, const TextureFormat format)
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
    std::shared_ptr<platform::stream::Stream>&& s, std::vector<std::uint8_t>&& data, const std::uint32_t img_width, const std::uint32_t img_height, const TextureFormat format, core::job::EndCaller<>&& end)
{
    core::job::send_job_to_pool([s = std::move(s), data = std::move(data), img_width, img_height, format, end = std::move(end)]() mutable {
        write_image(*s, data.data(), img_width, img_height, format);
        (void)end;
    });
}

gearoenix::render::texture::Texture::~Texture() = default;

void gearoenix::render::texture::Texture::write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>&, const bool include_content) const
{
    s->write_fail_debug(include_content);
    s->write_fail_debug(name);
    info.write(*s);
}

void gearoenix::render::texture::Texture::write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c) const { write(s, c, true); }

std::uint64_t gearoenix::render::texture::Texture::get_mipmaps_count() const { return compute_mipmaps_count(info); }

std::vector<std::uint8_t> gearoenix::render::texture::Texture::convert_pixels(const float* const data, const std::uint64_t in_components_count, const std::uint64_t pixels_count, const std::uint64_t out_components_count)
{
    auto iter = Pixel<float>::ConstIterator(data, in_components_count, pixels_count);
    const auto end = iter + (pixels_count + 1);
    std::vector<std::uint8_t> result(out_components_count * pixels_count);
    for (; iter != end; ++iter) {
        std::uint64_t i = 0;
        const std::uint64_t index = iter.get_pixel_index() * out_components_count;
        const float* const pixel_elements = iter.get_data();
        for (; i < in_components_count && i < out_components_count; ++i) {
            const auto c = static_cast<int>(pixel_elements[i] * 255.0f + 0.501f);
            result[index + i] = c >= 255 ? 255 : c <= 0.0 ? 0
                                                          : static_cast<std::uint8_t>(c);
        }
        for (; i < out_components_count; ++i) {
            result[index + i] = i < 4 ? 0 : 255;
        }
    }
    return result;
}

std::vector<std::vector<std::uint8_t>> gearoenix::render::texture::Texture::convert_float_pixels(const std::vector<std::vector<std::uint8_t>>& data, const std::uint64_t in_components_count, const std::uint64_t out_components_count)
{
    std::vector<std::vector<std::uint8_t>> result;
    result.reserve(data.size());
    const auto pixel_size = sizeof(float) * in_components_count;
    for (auto& d : data)
        result.push_back(convert_pixels(reinterpret_cast<const float*>(d.data()), in_components_count, d.size() / pixel_size, out_components_count));
    return result;
}

std::vector<std::vector<std::vector<std::uint8_t>>> gearoenix::render::texture::Texture::convert_float_pixels(
    const std::vector<std::vector<std::vector<std::uint8_t>>>& data, const std::uint64_t in_components_count, const std::uint64_t out_components_count)
{
    std::vector<std::vector<std::vector<std::uint8_t>>> result;
    result.reserve(data.size());
    for (auto& d : data)
        result.push_back(convert_float_pixels(d, in_components_count, out_components_count));
    return result;
}

std::uint64_t gearoenix::render::texture::Texture::compute_mipmaps_count(const std::uint64_t img_width, const std::uint64_t img_height) { return math::Numeric::floor_log2(img_width > img_height ? img_width : img_height) + 1; }

std::uint64_t gearoenix::render::texture::Texture::compute_mipmaps_count(const TextureInfo& info) { return info.get_has_mipmap() ? compute_mipmaps_count(info.get_width(), info.get_height()) : 1; }

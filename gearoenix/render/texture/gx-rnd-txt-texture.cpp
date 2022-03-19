#include "gx-rnd-txt-texture.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-txt-image.hpp"
#include "gx-rnd-txt-pixel-iterator.hpp"
#include <cmath>

gearoenix::render::texture::Texture::Texture(
    std::string name,
    const TextureInfo& info,
    engine::Engine& e) noexcept
    : e(e)
    , info(info)
    , name(std::move(name))
{
}

gearoenix::render::texture::Texture::~Texture() noexcept = default;

std::vector<std::uint8_t> gearoenix::render::texture::Texture::convert_pixels(
    const float* const data,
    const std::size_t in_components_count,
    const std::size_t pixels_count,
    const std::size_t out_components_count) noexcept
{
    auto iter = Pixel<float>::ConstIterator(data, in_components_count, pixels_count);
    auto end = iter + (pixels_count + 1);
    std::vector<std::uint8_t> result(out_components_count * pixels_count);
    for (; iter < end; ++iter) {
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
    const std::size_t out_components_count) noexcept
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
    const std::size_t out_components_count) noexcept
{
    std::vector<std::vector<std::vector<std::uint8_t>>> result;
    result.reserve(data.size());
    for (auto& d : data)
        result.push_back(convert_float_pixels(d, in_components_count, out_components_count));
    return result;
}

std::size_t gearoenix::render::texture::Texture::compute_mipmaps_count(
    const std::size_t img_width, const std::size_t img_height) noexcept
{
    return math::Numeric::floor_log2(img_width > img_height ? img_width : img_height) + 1;
}
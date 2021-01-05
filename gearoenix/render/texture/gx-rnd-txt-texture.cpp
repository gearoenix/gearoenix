#include "gx-rnd-txt-texture.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../system/stream/gx-sys-stm-local.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-txt-image.hpp"
#include "gx-rnd-txt-pixel-iterator.hpp"
#include <cmath>

void gearoenix::render::texture::Texture::write_gx3d_image(
    system::stream::Stream* const s,
    const float* const data,
    const std::size_t img_width,
    const std::size_t img_height,
    const std::size_t components_count) noexcept
{
    const auto offset_of_size = s->tell();
    (void)s->write(static_cast<core::Count>(0));
    const auto offset_after_size = s->tell();
    render::texture::Image::encode_hdr(s, data, img_width, img_height, components_count);
    const auto curr_off = s->tell();
    s->seek(offset_of_size);
    (void)s->write(static_cast<core::Count>(curr_off - offset_after_size));
    s->seek(curr_off);
}

void gearoenix::render::texture::Texture::write_gx3d_image(
    system::stream::Stream* const s,
    const unsigned char* const data,
    const std::size_t img_width,
    const std::size_t img_height,
    const std::size_t components_count) noexcept
{
    const auto offset_of_size = s->tell();
    (void)s->write(static_cast<core::Count>(0));
    const auto offset_after_size = s->tell();
    render::texture::Image::encode_png(s, data, img_width, img_height, components_count);
    const auto curr_off = s->tell();
    s->seek(offset_of_size);
    (void)s->write(static_cast<core::Count>(curr_off - offset_after_size));
    s->seek(curr_off);
}

gearoenix::render::texture::Texture::Texture(
    const core::Id my_id,
    std::string name,
    const Type texture_type,
    const TextureFormat texture_format,
    const SamplerInfo& sampler_info,
    engine::Engine* const e) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Texture, std::move(name))
    , texture_type(texture_type)
    , texture_format(texture_format)
    , render_engine(e)
    , sample_info(sampler_info)
{
}

gearoenix::render::texture::Texture::~Texture() noexcept = default;

void gearoenix::render::texture::Texture::write_gx3d(
    const std::string& file_address,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    std::shared_ptr<system::stream::Stream> l(new system::stream::Local(render_engine->get_system_application(), file_address, true));
    write_gx3d(l, c);
}

void gearoenix::render::texture::Texture::write_gx3d(
    const std::shared_ptr<system::stream::Stream>& s,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
{
    (void)s->write(texture_type);
    (void)s->write(texture_format);
    sample_info.write(s.get());
}

std::vector<std::uint8_t> gearoenix::render::texture::Texture::convert_pixels(
    const float* const data,
    const std::size_t in_components_count,
    const std::size_t pixels_count,
    const std::size_t out_components_count) noexcept
{
    auto begin = Pixel<float>::ConstIterator(data, in_components_count, pixels_count);
    auto end = begin + (pixels_count + 1);
    std::vector<std::uint8_t> result(out_components_count * pixels_count);
    core::sync::ParallelFor::exec(begin, end, [in_components_count, out_components_count, &result](const decltype(begin)& iter) {
        std::size_t i = 0;
        auto result_index = iter.get_pixel_index() * out_components_count;
        for (; i < in_components_count && i < out_components_count; ++i) {
            const float c = std::round((iter[i] * 255.0f)) + 0.1f;
            result[result_index] = c >= 255.0 ? 255 : c <= 0.0 ? 0
                                                               : static_cast<std::uint8_t>(c);
            ++result_index;
        }
        for (; i < out_components_count; ++i) {
            result[result_index] = i < 4 ? 0 : 255;
            ++result_index;
        }
    });
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
    return math::Numeric::floor_log2(GX_MAX(img_width, img_height)) + 1;
}
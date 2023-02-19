#include "gx-rnd-fnt-font.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../core/gx-cr-string.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include <cmath>
#include <string>

#ifdef GX_DEBUG_MODE
// #define GX_DEBUG_FONT_IMAGE_CREATION
#endif

#ifdef GX_IN_WINDOWS
#define STBI_MSC_SECURE_CRT
#endif

#ifdef GX_DEBUG_FONT_IMAGE_CREATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#else
#define STBI_WRITE_NO_STDIO
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#ifdef GX_DEBUG_FONT_IMAGE_CREATION
static int file_index = 0;
#endif

void gearoenix::render::font::Font::init() noexcept
{
    const auto init_result = stbtt_InitFont(stb_font.get(), ttf_data.data(), 0);
    GX_ASSERT_D(0 != init_result);
    stbtt_GetFontVMetrics(stb_font.get(), &ascent, &descent, &line_gap);
    fnt_height = ascent - descent;
    line_growth = line_gap + fnt_height;
}

gearoenix::render::font::Font::Font(
    engine::Engine& e,
    const std::string& name) noexcept
    : e(e)
    , stb_font(new stbtt_fontinfo())
    , ttf_data(platform::stream::Asset::construct(e.get_platform_application(), "fonts/" + name + ".ttf")->get_file_content())
{
    init();
}

gearoenix::render::font::Font::~Font() noexcept = default;

void gearoenix::render::font::Font::compute_text_widths(
    const std::wstring& text,
    const double text_height,
    std::vector<double>& widths) const noexcept
{
#ifdef GX_DEBUG_MODE
    if (text.empty()) {
        GX_UNEXPECTED;
    }
    for (const auto c : text) {
        if (c == static_cast<int>('\n')) {
            GX_UNEXPECTED;
        }
    }
#endif
    const auto txt_scale = text_height / static_cast<double>(fnt_height);
    widths.clear();
    double total_width;
    {
        int advance = 0;
        int lsb = 0;
        stbtt_GetCodepointHMetrics(stb_font.get(), text[0], &advance, &lsb);
        total_width = static_cast<double>(std::abs(lsb)) * txt_scale;
        widths.push_back(total_width);
        total_width += static_cast<double>(advance) * txt_scale;
        widths.push_back(total_width);
    }
    const auto end_index = text.size() - 1;
    for (std::size_t i = 0; i < end_index;) {
        const auto pre_c = static_cast<int>(static_cast<unsigned char>(text[i]));
        const auto cur_c = static_cast<int>(static_cast<unsigned char>(text[++i]));
        total_width += static_cast<double>(stbtt_GetCodepointKernAdvance(stb_font.get(), pre_c, cur_c)) * txt_scale;
        int advance = 0;
        int lsb = 0;
        stbtt_GetCodepointHMetrics(stb_font.get(), cur_c, &advance, &lsb);
        const auto current_lsb = static_cast<double>(std::abs(lsb)) * txt_scale;
        if (current_lsb > total_width) {
            const auto addition = current_lsb - total_width;
            for (auto& w : widths) {
                w += addition;
            }
            total_width = current_lsb;
        }
        total_width += static_cast<double>(advance) * txt_scale;
        widths.push_back(total_width);
    }
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::font::Font::bake(
    const std::wstring& text,
    const std::vector<double>& txt_widths,
    const math::Vec4<double>& color_vector,
    const double img_width,
    const double img_height,
    const double img_start_skip,
    const core::sync::EndCaller& end) const noexcept
{
    const auto converter = [](const double v) {
        if (v >= 1.0f)
            return static_cast<std::uint8_t>(255);
        if (v <= 0.0f)
            return static_cast<std::uint8_t>(0);
        return static_cast<std::uint8_t>(v * 255.0f + 0.5000001f);
    };
    const std::array<std::uint8_t, 4> color_bytes {
        converter(color_vector.x),
        converter(color_vector.y),
        converter(color_vector.z),
        converter(color_vector.w),
    };
    const auto max_texture_size = e.get_specification().texture_maximum_aspect;
    const auto window_height = e.get_platform_application().get_base().get_window_size().y;
    const auto img_height_pixels = math::Numeric::raise_p2(
        static_cast<unsigned int>(std::ceil(static_cast<double>(window_height) * img_height)),
        max_texture_size, 16U);
    const auto h_scale = static_cast<double>(img_height_pixels) / static_cast<double>(fnt_height);
    const auto img_width_pixels = math::Numeric::raise_p2(
        static_cast<unsigned int>(std::ceil(static_cast<double>(window_height) * img_width)),
        max_texture_size, 16U);
    const auto w_scale = (static_cast<double>(img_width_pixels) * img_height) / (img_width * static_cast<double>(fnt_height));
    const auto width_to_pixel = static_cast<double>(img_width_pixels) / img_width;
    std::size_t index = 0;
    while (txt_widths[index] < img_start_skip)
        ++index;
    const auto base_line = static_cast<double>(ascent) * h_scale;
    std::vector<unsigned char> rnd_data(static_cast<std::size_t>(img_width_pixels) * static_cast<std::size_t>(img_height_pixels));
    const auto x_pos_start = (txt_widths[index] - img_start_skip) * width_to_pixel;
    const auto txt_end_index = text.size() - 1;
    const auto txt_end_width = img_start_skip + img_width;
    auto x_pos = x_pos_start;
    const auto get_index = [&](const int x, const int y) {
        return (static_cast<int>(base_line) + y) * img_width_pixels + x + static_cast<int>(x_pos);
    };
    const auto y_shift = base_line - floor(base_line);
    while (index < txt_end_index) {
        const wchar_t c = text[index];
        ++index;
        if (txt_widths[index] > txt_end_width)
            break;
        const wchar_t next_c = text[index];
        int advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        const auto x_shift = x_pos - floor(x_pos);
        stbtt_GetCodepointHMetrics(stb_font.get(), c, &advance, &lsb);
        stbtt_GetCodepointBitmapBoxSubpixel(stb_font.get(), c, static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), &x0, &y0, &x1, &y1);
        stbtt_MakeCodepointBitmapSubpixel(stb_font.get(), &rnd_data[get_index(x0, y0)], x1 - x0, y1 - y0, static_cast<int>(img_width_pixels), static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), c);
        x_pos += w_scale * static_cast<double>(advance + stbtt_GetCodepointKernAdvance(stb_font.get(), c, next_c));
    }
    if (txt_widths[index] <= txt_end_width) {
        const wchar_t c = text[index];
        int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        const auto x_shift = x_pos - floor(x_pos);
        stbtt_GetCodepointBitmapBoxSubpixel(stb_font.get(), c, static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), &x0, &y0, &x1, &y1);
        stbtt_MakeCodepointBitmapSubpixel(stb_font.get(), &rnd_data[get_index(x0, y0)], x1 - x0, y1 - y0, static_cast<int>(img_width_pixels), static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), c);
    }

#ifdef GX_DEBUG_FONT_IMAGE_CREATION
    stbi_write_png((std::string("font2d-img-") + std::to_string(++file_index)).c_str(), img_width_pixels, img_height_pixels, 1, rnd_data.data(), 0);
#endif

    const auto pixels_bytes_count = rnd_data.size() * 4;
    std::vector<std::uint8_t> img_pixels(pixels_bytes_count);
    for (std::size_t i = 0, ti = 0, img_index = 0; i < static_cast<std::size_t>(img_height_pixels); ++i) {
        for (std::size_t j = 0; j < static_cast<std::size_t>(img_width_pixels); ++j, ++ti, ++img_index) {
            const auto c = static_cast<unsigned int>(rnd_data[ti]);
            img_pixels[img_index] = color_bytes[0];
            img_pixels[++img_index] = color_bytes[1];
            img_pixels[++img_index] = color_bytes[2];
            if (c == 0) {
                img_pixels[++img_index] = 0;
            } else {
                img_pixels[++img_index] = static_cast<unsigned char>((c * static_cast<unsigned int>(color_bytes[3])) / 255);
            }
        }
    }
    const texture::TextureInfo txt_info {
        .format = texture::TextureFormat::RgbaUint8,
        .sampler_info = texture::SamplerInfo {
            .min_filter = texture::Filter::LinearMipmapLinear,
            .mag_filter = texture::Filter::Linear,
            .wrap_s = texture::Wrap::ClampToEdge,
            .wrap_t = texture::Wrap::ClampToEdge,
            .wrap_r = texture::Wrap::ClampToEdge,
        },
        .width = img_width_pixels,
        .height = img_height_pixels,
        .type = texture::Type::Texture2D,
        .has_mipmap = false,
    };
    return e.get_texture_manager()->create_2d_from_pixels(
        "gearoenix-baked-tex2d-" + core::String::to_string(text),
        { std::move(img_pixels) }, txt_info, end);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::font::Font::bake(
    const std::wstring& text,
    const math::Vec4<double>& color,
    const double img_height,
    double& img_width,
    const core::sync::EndCaller& end) const noexcept
{
    if (text.empty()) {
        img_width = img_height;
        return e.get_texture_manager()->create_2d_from_colour(math::Vec4(0.0f), end);
    }
    std::vector<double> widths(text.size() + 1);
    compute_text_widths(text, img_height, widths);
    img_width = widths.back();
    return bake(text, widths, color, img_width, img_height, 0.0f, end);
}

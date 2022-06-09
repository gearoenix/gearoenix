#include "gx-rnd-fnt-2d.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include <cmath>
#include <string>

#ifdef GX_DEBUG_MODE
//#define GX_DEBUG_FONT_IMAGE_CREATION
#endif

#ifdef GX_IN_WINDOWS
#define STBI_MSC_SECURE_CRT
#endif
#ifdef GX_DEBUG_FONT_IMAGE_CREATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#else
#define STBI_WRITE_NO_STDIO
#endif
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#ifdef GX_DEBUG_FONT_IMAGE_CREATION
static int file_index = 0;
#endif

void gearoenix::render::font::Font2D::init() noexcept
{
    GX_CHECK_NOT_EQUAL_D(0, stbtt_InitFont(stb_font.get(), ttf_data.data(), 0))
    stbtt_GetFontVMetrics(stb_font.get(), &ascent, &descent, &line_gap);
    fnt_height = ascent - descent;
    line_growth = line_gap + fnt_height;
}

gearoenix::render::font::Font2D::Font2D(
    const core::Id id,
    std::string name,
    texture::Manager* const txt_mgr) noexcept
    : Font(id, std::move(name), Type::D2)
    , txt_mgr(txt_mgr)
    , stb_font(new stbtt_fontinfo())
{
}

gearoenix::render::font::Font2D::Font2D(
    const core::Id my_id,
    std::string name,
    platform::stream::Stream* f,
    texture::Manager* const txt_mgr) noexcept
    : Font(my_id, std::move(name), Type::D2)
    , txt_mgr(txt_mgr)
    , stb_font(new stbtt_fontinfo())
{
    f->read(ttf_data);
    init();
}

std::shared_ptr<gearoenix::render::font::Font2D> gearoenix::render::font::Font2D::construct_default(
    const core::Id id,
    std::string name,
    texture::Manager* const txt_mgr) noexcept
{
    auto* const e = txt_mgr->get_engine();
    const std::unique_ptr<platform::stream::Asset> asset(platform::stream::Asset::construct(
        e->get_platform_application(), "default.ttf"));
    if (asset == nullptr) {
        GXLOGD("Default font-2d not found.")
        return nullptr;
    }
    std::shared_ptr<Font2D> result(new Font2D(id, std::move(name), txt_mgr));
    const auto s = asset->size();
    result->ttf_data.resize(static_cast<std::size_t>(s));
    asset->seek(0);
    (void)asset->read(result->ttf_data.data(), s);
    result->init();
    return result;
}

gearoenix::render::font::Font2D::~Font2D() noexcept
{
    stb_font = nullptr;
}

void gearoenix::render::font::Font2D::compute_text_widths(
    const std::wstring& text,
    const double text_height,
    std::vector<double>& widths) const noexcept
{
#ifdef GX_DEBUG_MODE
    if (text.empty()) {
        GX_UNEXPECTED
    }
    for (const auto c : text) {
        if (c == static_cast<int>('\n')) {
            GX_UNEXPECTED
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

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::font::Font2D::bake(
    const std::wstring& text,
    const std::vector<double>& txt_widths,
    const math::Vec4<double>& color_vector,
    const double img_width,
    const double img_height,
    const double img_start_skip,
    core::sync::EndCaller<texture::Texture2D>& end) const noexcept
{
    const auto converter = [](const double v) {
        if (v >= 1.0f)
            return static_cast<std::uint8_t>(255);
        if (v <= 0.0f)
            return static_cast<std::uint8_t>(0);
        return static_cast<std::uint8_t>(v * 255.0f + 0.5000001f);
    };
    const std::uint8_t color_bytes[4] = {
        converter(color_vector.x),
        converter(color_vector.y),
        converter(color_vector.z),
        converter(color_vector.w),
    };
    auto* const render_engine = txt_mgr->get_engine();
    const auto max_texture_size = render_engine->get_limitations().texture_maximum_aspect;
    const auto window_height = render_engine->get_platform_application()->get_event_engine()->get_window_height();
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
        int advance, lsb, x0, y0, x1, y1;
        const auto x_shift = x_pos - floor(x_pos);
        stbtt_GetCodepointHMetrics(stb_font.get(), c, &advance, &lsb);
        stbtt_GetCodepointBitmapBoxSubpixel(stb_font.get(), c, static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), &x0, &y0, &x1, &y1);
        stbtt_MakeCodepointBitmapSubpixel(stb_font.get(), &rnd_data[get_index(x0, y0)], x1 - x0, y1 - y0, img_width_pixels, static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), c);
        x_pos += w_scale * static_cast<double>(advance + stbtt_GetCodepointKernAdvance(stb_font.get(), c, next_c));
    }
    if (txt_widths[index] <= txt_end_width) {
        const wchar_t c = text[index];
        int x0, y0, x1, y1;
        const auto x_shift = x_pos - floor(x_pos);
        stbtt_GetCodepointBitmapBoxSubpixel(stb_font.get(), c, static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), &x0, &y0, &x1, &y1);
        stbtt_MakeCodepointBitmapSubpixel(stb_font.get(), &rnd_data[get_index(x0, y0)], x1 - x0, y1 - y0, img_width_pixels, static_cast<float>(w_scale), static_cast<float>(h_scale), static_cast<float>(x_shift), static_cast<float>(y_shift), c);
    }

#ifdef GX_DEBUG_FONT_IMAGE_CREATION
    stbi_write_png((std::string("font2d-img-") + std::to_string(++file_index)).c_str(), img_width_pixels, img_height_pixels, 1, rnd_data.data(), 0);
#endif

    const auto pixels_bytes_count = rnd_data.size() * 4;
    std::vector<std::uint8_t> img_pixels(pixels_bytes_count);
    for (std::size_t i = 0, ti = 0, img_index = (static_cast<std::size_t>(img_height_pixels) - 1) * static_cast<std::size_t>(img_width_pixels) * 4;
         i < static_cast<std::size_t>(img_height_pixels);
         ++i, img_index -= (static_cast<std::size_t>(img_width_pixels) * 8)) {
        for (std::size_t j = 0; j < static_cast<std::size_t>(img_width_pixels); ++j, ++ti) {
            const auto c = static_cast<unsigned int>(rnd_data[ti]);
            if (c == 0) {
                img_pixels[img_index++] = 0;
                img_pixels[img_index++] = 0;
                img_pixels[img_index++] = 0;
                img_pixels[img_index++] = 0;
            } else {
                img_pixels[img_index++] = color_bytes[0];
                img_pixels[img_index++] = color_bytes[1];
                img_pixels[img_index++] = color_bytes[2];
                img_pixels[img_index++] = static_cast<unsigned char>((c * static_cast<unsigned int>(color_bytes[3])) / 255);
            }
        }
    }
    constexpr texture::TextureInfo txt_info {
        .format = texture::TextureFormat::RgbaUint8,
        .sampler_info = texture::SamplerInfo {
            .min_filter = texture::Filter::LinearMipmapLinear,
            .mag_filter = texture::Filter::Linear,
            .wrap_s = texture::Wrap::ClampToEdge,
            .wrap_t = texture::Wrap::ClampToEdge,
            .wrap_r = texture::Wrap::ClampToEdge,
        },
        .texture_type = texture::Type::Texture2D,
    };
    return txt_mgr->create_2d(
        "baked-tex2d-" + std::to_string(core::asset::Manager::create_id()),
        { std::move(img_pixels) }, txt_info, img_width_pixels, img_height_pixels, end);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::font::Font2D::bake(
    const std::wstring& text,
    const math::Vec4<double>& color,
    const double img_height,
    double& img_width,
    core::sync::EndCaller<texture::Texture2D>& end) const noexcept
{
    if (text.empty()) {
        img_width = img_height;
        return txt_mgr->get_2d(math::Vec4(0.0f), end);
    }
    std::vector<double> widths(text.size() + 1);
    compute_text_widths(text, img_height, widths);
    img_width = widths.back();
    return bake(text, widths, color, img_width, img_height, 0.0f, end);
}

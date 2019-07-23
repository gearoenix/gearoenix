#include "rnd-fnt-2d.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-static.hpp"
#include "../../system/stream/sys-stm-local.hpp"
#include "../../system/stream/sys-stm-memory.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"
#ifdef GX_IN_WINDOWS
#define STBI_MSC_SECURE_CRT
#endif
#define STBI_WRITE_NO_STDIO
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <cmath>
#include <string>

gearoenix::render::font::Font2D::MultilineTextAspectsInfo gearoenix::render::font::Font2D::compute_multiline_text_aspects(const std::wstring& text) const noexcept
{
	bool start_of_line = true;
	MultilineTextAspectsInfo a;
	int w = 1;
	a.height = line_growth + 1;
	const std::size_t txt_size_less = text.size() - 1;
	for (std::size_t i = 0; i < txt_size_less;)
	{
		const wchar_t c = text[i];
		++i;
		const wchar_t next_c = text[i];
		if (c == '\n')
		{
			a.height += line_growth;
			w = 1;
			continue;
		}
		int advance = 0;
		int lsb = 0;
		stbtt_GetCodepointHMetrics(stb_font, c, &advance, &lsb);
		lsb = w - lsb;
		if (a.max_lsb > lsb)
			a.max_lsb = lsb;
		w += advance;
		w += stbtt_GetCodepointKernAdvance(stb_font, c, next_c);
		if (a.max_width < w)
			a.max_width = w;
	}
	{
		const wchar_t c = text[txt_size_less];
		if (c == '\n')
		{
			a.height += line_growth;
		}
		else
		{
			int advance = 0;
			int lsb = 0;
			stbtt_GetCodepointHMetrics(stb_font, c, &advance, &lsb);
			lsb = w - lsb;
			if (a.max_lsb > lsb)
				a.max_lsb = lsb;
			w += advance;
			if (a.max_width < w)
				a.max_width = w;
		}
	}
	a.max_lsb = -a.max_lsb;
	a.max_width += a.max_lsb;
	return a;
}

gearoenix::render::font::Font2D::Font2D(
	const core::Id my_id,
	system::stream::Stream* f,
	std::shared_ptr<texture::Manager> txt_mgr) noexcept
    : Font(my_id, Type::D2)
	, stb_font(new stbtt_fontinfo())
{
	std::vector<unsigned char> ttf_data;
    f->read(ttf_data);
	GX_CHECK_NOT_EQAUL_D(0, stbtt_InitFont(stb_font, ttf_data.data(), 0))
}

gearoenix::render::font::Font2D::~Font2D()
{
	delete stb_font;
}

const std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::font::Font2D::multiline_bake(
	const std::wstring& text, 
	const std::uint32_t color, 
	const int img_width, 
	const int img_height, 
	const int img_margin, 
	core::Real& render_aspect_ratio) const noexcept
{
	const auto a = compute_multiline_text_aspects(text);
	const core::Real w_scale = static_cast<core::Real>(img_width - (img_margin << 1)) / static_cast<core::Real>(a.max_width);
	const core::Real h_scale = static_cast<core::Real>(img_height - (img_margin << 1)) / static_cast<core::Real>(a.height);
	render_aspect_ratio = (w_scale * static_cast<core::Real>(img_margin << 1) + static_cast<core::Real>(a.max_width)) / 
		(h_scale * static_cast<core::Real>(img_margin << 1) + static_cast<core::Real>(a.height));

	core::Real base_line = static_cast<core::Real>(ascent) * h_scale + static_cast<core::Real>(img_margin);
	std::vector<unsigned char> rnd_data(img_width * img_height);

	const core::Real scaled_lg = static_cast<core::Real>(line_growth) * h_scale;
	const core::Real xpos_start = static_cast<core::Real>(a.max_lsb) * w_scale + static_cast<core::Real>(img_margin);
	const std::size_t txt_size_less = text.size() - 1;
	core::Real xpos = xpos_start;

	const auto index = [&](const int x, const int y) {
		return y * img_width + x;
	};

	core::Real y_shift = base_line - static_cast<core::Real>(floor(base_line));

	for (std::size_t i = 0; i < txt_size_less;)
	{
		const wchar_t c = text[i];
		++i;
		const wchar_t next_c = text[i];
		if (c == '\n')
		{
			base_line += scaled_lg;
			xpos = xpos_start;
			y_shift = base_line - static_cast<core::Real>(floor(base_line));
			continue;
		}
		int advance, lsb, x0, y0, x1, y1;
		const core::Real x_shift = xpos - static_cast<core::Real>(floor(xpos));
		stbtt_GetCodepointHMetrics(stb_font, c, &advance, &lsb);
		stbtt_GetCodepointBitmapBoxSubpixel(stb_font, c, w_scale, h_scale, x_shift, y_shift, &x0, &y0, &x1, &y1);
		stbtt_MakeCodepointBitmapSubpixel(stb_font, &rnd_data[index(int(xpos) + x0, int(base_line) + y0)], x1 - x0, y1 - y0, img_width, w_scale, h_scale, x_shift, y_shift, c);
		xpos += w_scale * (advance + stbtt_GetCodepointKernAdvance(stb_font, c, next_c));
	}
	{
		const wchar_t c = text[txt_size_less];
		if (c != '\n')
		{
			int x0, y0, x1, y1;
			const core::Real x_shift = xpos - static_cast<core::Real>(floor(xpos));
			stbtt_GetCodepointBitmapBoxSubpixel(stb_font, c, w_scale, h_scale, x_shift, y_shift, &x0, &y0, &x1, &y1);
			stbtt_MakeCodepointBitmapSubpixel(stb_font, &rnd_data[index(int(xpos) + x0, int(base_line) + y0)], x1 - x0, y1 - y0, img_width, w_scale, h_scale, x_shift, y_shift, c);
		}
	}

	auto color_bytes = reinterpret_cast<const unsigned char*>(&color);

	std::vector<unsigned char> img_pixels(img_width * img_height << 2);
	int img_index = 0;
	for (const unsigned char c : rnd_data)
	{
		if (c == 0)
		{
			img_pixels[img_index++] = 0;
			img_pixels[img_index++] = 0;
			img_pixels[img_index++] = 0;
			img_pixels[img_index++] = 0;
		}
		else
		{
			img_pixels[img_index++] = color_bytes[0];
			img_pixels[img_index++] = color_bytes[1];
			img_pixels[img_index++] = color_bytes[2];
			img_pixels[img_index++] = static_cast<unsigned char>((static_cast<int>(c) * static_cast<int>(color_bytes[3])) >> 8);
		}
	}
	GXUNIMPLEMENTED
	return nullptr;
}

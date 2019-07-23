#include "rnd-fnt-2d.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
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

gearoenix::render::font::Font2D::Font2D(
	const core::Id my_id,
	system::stream::Stream* f,
	std::shared_ptr<texture::Manager> txt_mgr) noexcept
    : Font(my_id, Type::D2)
	, stb_font(new stbtt_fontinfo())
{
	std::vector<unsigned char> ttf_data;
    f->read(ttf_data);
#ifdef GX_DEBUG_MODE
    if (0 ==
#endif
        stbtt_InitFont(stb_font, ttf_data.data(), 0)
#ifdef GX_DEBUG_MODE
        ) GXLOGF("TTF font failed to load: " << my_id)
#else
        ;
#endif
}

gearoenix::render::font::Font2D::~Font2D()
{
	delete stb_font;
}
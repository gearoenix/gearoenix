#ifndef GEAROENIX_RENDER_FONT_2D_HPP
#define GEAROENIX_RENDER_FONT_2D_HPP
#include "../../math/math-vector.hpp"
#include "rnd-fnt-font.hpp"
#include <vector>
#include <memory>
#include <string>

struct stbtt_fontinfo;

namespace gearoenix::render::texture {
class Manager;
class Texture2D;
}

namespace gearoenix::render::font {
class Font2D : public Font {
private:
	struct MultilineTextAspectsInfo {
		int max_lsb = 0;
		int max_width = 1;
		int height = 1;
	};
    const std::shared_ptr<texture::Manager> txt_mgr;

    std::vector<unsigned char> ttf_data;
	stbtt_fontinfo *stb_font = nullptr;
	int ascent = 0;
	int descent = 0;
	int line_gap = 0;
	int line_growth = 0;
	int fnt_height = 0;

	MultilineTextAspectsInfo compute_multiline_text_aspects(const std::wstring& text) const noexcept;
    void init() noexcept;

public:
    Font2D(
        const core::Id my_id,
        system::stream::Stream* f,
        const std::shared_ptr<texture::Manager>& txt_mgr) noexcept;
    Font2D(
        const core::Id my_id,
        const std::shared_ptr<texture::Manager>& txt_mgr) noexcept;
    ~Font2D() noexcept;
	/// This function is a simple and fast way to only render a multiline text to a texture.
	/// It is not a good way to handle large text area with this function.
	/// In future, maybe, I will implement a one line fast enough texture baker.
    const std::shared_ptr<texture::Texture2D> multiline_bake(
		const std::wstring &text,
        std::uint32_t color,
        int img_width,
        int img_height,
		int img_margin,
        core::Real &render_aspect_ratio,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& end = core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})) const noexcept;
};
}
#endif
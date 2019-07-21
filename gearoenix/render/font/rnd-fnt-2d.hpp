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
	stbtt_fontinfo *stb_font = nullptr;

public:
    Font2D(
        const core::Id my_id,
		system::stream::Stream* f,
		std::shared_ptr<texture::Manager> txt_mgr) noexcept;
    ~Font2D() noexcept;
    const std::shared_ptr<texture::Texture2D> bake(const std::wstring &text) const noexcept;
};
}
#endif
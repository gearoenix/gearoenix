#ifndef GEAROENIX_RENDER_FONT_2D_HPP
#define GEAROENIX_RENDER_FONT_2D_HPP
#include "../../math/math-vector.hpp"
#include "rnd-fnt-font.hpp"
#include <memory>
#include <string>
#include <vector>

struct stbtt_fontinfo;

namespace gearoenix::render::texture {
class Manager;
class Texture2D;
}

namespace gearoenix::render::font {
class Font2D : public Font {
private:
    struct TextAspectsInfo {
        unsigned int lsb = 0;
        unsigned int width = 0;
        unsigned int height = 0;
    };
    texture::Manager* const txt_mgr;

    std::vector<unsigned char> ttf_data;
    std::unique_ptr<stbtt_fontinfo> stb_font;
    int ascent = 0;
    int descent = 0;
    int line_gap = 0;
    int line_growth = 0;
    int fnt_height = 0;

    [[nodiscard]] TextAspectsInfo compute_text_aspects(const std::wstring& text) const noexcept;
    void init() noexcept;

public:
    Font2D(
        core::Id my_id,
        system::stream::Stream* f,
        texture::Manager* txt_mgr) noexcept;
    Font2D(
        core::Id my_id,
        texture::Manager* txt_mgr) noexcept;
    ~Font2D() noexcept final;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    [[nodiscard]] std::shared_ptr<texture::Texture2D> bake(
        const std::wstring& text,
        const std::uint8_t color[4],
        core::Real scale,
        unsigned int img_margin,
        core::Real& render_aspect_ratio,
        core::Real& starting_aspect_ratio,
        core::sync::EndCaller<texture::Texture2D>& end) const noexcept;
};
}
#endif
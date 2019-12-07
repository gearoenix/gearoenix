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
    texture::Manager* const txt_mgr;
    std::vector<unsigned char> ttf_data;
    std::unique_ptr<stbtt_fontinfo> stb_font;
    int ascent = 0;
    int descent = 0;
    int line_gap = 0;
    int line_growth = 0;
    int fnt_height = 0;

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
    /// This function returns n+1 horizontal length from start
    void compute_text_widths(const std::wstring& text, core::Real text_height, std::vector<core::Real>& widths) const noexcept;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    [[nodiscard]] std::shared_ptr<texture::Texture2D> bake(
        const std::wstring& text,
        const std::vector<core::Real>& txt_widths,
        const math::Vec4& color,
        core::Real img_width,
        core::Real img_height,
        core::Real img_start_skip,
        core::sync::EndCaller<texture::Texture2D>& end) const noexcept;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    [[nodiscard]] std::shared_ptr<texture::Texture2D> bake(
        const std::wstring& text,
        const math::Vec4& color,
        core::Real img_height,
        core::Real& img_width,
        core::sync::EndCaller<texture::Texture2D>& end) const noexcept;
};
}
#endif
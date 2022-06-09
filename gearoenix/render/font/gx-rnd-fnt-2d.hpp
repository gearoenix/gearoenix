#ifndef GEAROENIX_RENDER_FONT_2D_HPP
#define GEAROENIX_RENDER_FONT_2D_HPP
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-fnt-font.hpp"
#include <memory>
#include <string>
#include <vector>

struct stbtt_fontinfo;

namespace gearoenix::render::texture {
struct Manager;
struct Texture2D;
}

namespace gearoenix::render::font {
struct Font2D final : public Font {
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

    Font2D(core::Id id, std::string name, texture::Manager* txt_mgr) noexcept;

public:
    Font2D(
        core::Id my_id,
        std::string name,
        platform::stream::Stream* f,
        texture::Manager* txt_mgr) noexcept;
    [[nodiscard]] static std::shared_ptr<Font2D> construct_default(
        core::Id id,
        std::string name,
        texture::Manager* txt_mgr) noexcept;
    ~Font2D() noexcept final;
    /// This function returns n+1 horizontal length from start
    void compute_text_widths(const std::wstring& text, double text_height, std::vector<double>& widths) const noexcept;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    [[nodiscard]] std::shared_ptr<texture::Texture2D> bake(
        const std::wstring& text,
        const std::vector<double>& txt_widths,
        const math::Vec4<double>& color,
        double img_width,
        double img_height,
        double img_start_skip,
        core::sync::EndCaller<texture::Texture2D>& end) const noexcept;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    [[nodiscard]] std::shared_ptr<texture::Texture2D> bake(
        const std::wstring& text,
        const math::Vec4<double>& color,
        double img_height,
        double& img_width,
        core::sync::EndCaller<texture::Texture2D>& end) const noexcept;
};
}
#endif
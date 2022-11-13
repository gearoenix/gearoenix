#ifndef GEAROENIX_RENDER_FONT_FONT_HPP
#define GEAROENIX_RENDER_FONT_FONT_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include <memory>
#include <string>
#include <vector>

struct stbtt_fontinfo;

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Manager;
struct Texture2D;
}

namespace gearoenix::render::font {
struct Font final {
    engine::Engine& e;

private:
    const std::vector<unsigned char> ttf_data;
    const std::unique_ptr<stbtt_fontinfo> stb_font;
    int ascent = 0;
    int descent = 0;
    int line_gap = 0;
    int line_growth = 0;
    int fnt_height = 0;

    void init() noexcept;

public:
    Font(
        engine::Engine& e,
        const std::string& name) noexcept;
    ~Font() noexcept;
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
        const core::sync::EndCallerIgnored& end) const noexcept;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    [[nodiscard]] std::shared_ptr<texture::Texture2D> bake(
        const std::wstring& text,
        const math::Vec4<double>& color,
        double img_height,
        double& img_width,
        const core::sync::EndCallerIgnored& end) const noexcept;
};
}
#endif
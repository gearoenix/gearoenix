#ifndef GEAROENIX_RENDER_FONT_FONT_HPP
#define GEAROENIX_RENDER_FONT_FONT_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
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
    const std::unique_ptr<stbtt_fontinfo> stb_font;
    const std::vector<unsigned char> ttf_data;
    int ascent = 0;
    int descent = 0;
    int line_gap = 0;
    int line_growth = 0;
    int fnt_height = 0;

    void init();

public:
    Font(
        engine::Engine& e,
        const std::string& name);
    ~Font();
    /// This function returns n+1 horizontal length from start
    void compute_text_widths(const std::wstring& text, double text_height, std::vector<double>& widths) const;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    void bake(
        const std::wstring& text,
        const std::vector<double>& txt_widths,
        const math::Vec4<double>& color,
        double img_width,
        double img_height,
        double img_start_skip,
        core::job::EndCallerShared<texture::Texture2D>&& end) const;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    void bake(
        const std::wstring& text,
        const math::Vec4<double>& color,
        double img_height,
        double& img_width,
        core::job::EndCallerShared<texture::Texture2D>&& end) const;
};
}
#endif
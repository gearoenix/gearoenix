#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include <memory>
#include <string>
#include <vector>

struct stbtt_fontinfo;

namespace gearoenix::render::texture {
struct Manager;
struct Texture2D;
}

namespace gearoenix::render::font {
struct Font final {
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
    explicit Font(const std::string& name);
    ~Font();
    /// This function returns n+1 horizontal length from start
    void compute_text_widths(const std::wstring& text, core::fp_t text_height, std::vector<core::fp_t>& widths) const;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    void bake(const std::wstring& text, const std::vector<core::fp_t>& txt_widths, const math::Vec4<core::fp_t>& color, core::fp_t img_width, core::fp_t img_height, core::fp_t img_start_skip, core::job::EndCallerShared<texture::Texture2D>&& end) const;
    /// This function is a simple and fast way to only render a one-line text to a texture.
    void bake(const std::wstring& text, const math::Vec4<core::fp_t>& color, core::fp_t img_height, core::fp_t& img_width, core::job::EndCallerShared<texture::Texture2D>&& end) const;
};
}
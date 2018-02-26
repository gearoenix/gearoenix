#include "../gearoenix/core/cr-end-caller.hpp"
#include "../gearoenix/math/math-vector.hpp"
#include "../gearoenix/render/font/rnd-fnt-2d.hpp"
#ifdef IN_WINDOWS
#define STBI_MSC_SECURE_CRT
#endif
#define STBI_WRITE_NO_STDIO
#define STB_TRUETYPE_IMPLEMENTATION
#include "../external/stb/stb_truetype.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

static void png_write_function(void* context, void* data, int size)
{
    std::ofstream* baked_file = reinterpret_cast<std::ofstream*>(context);
    baked_file->write(reinterpret_cast<const char*>(data), size);
}

void terminate(const std::string& msg)
{
    std::cerr << msg << std::endl;
    std::terminate();
}

int main(const int argc, const char* const* const argv)
{
    if (argc < 3)
        terminate("Wrong usage.");
    std::vector<gearoenix::render::font::Font2D::LetterProperties> lps(gearoenix::render::font::Font2D::characters_count);
    const int root_nchar = (int)std::ceil(std::sqrt((float)gearoenix::render::font::Font2D::characters_count));
    std::vector<stbtt_bakedchar> bkchar(gearoenix::render::font::Font2D::characters_count);
    std::ifstream file(argv[1], std::ios::binary | std::ios::in);
    if (!file.is_open())
        terminate("Input file did't open successfully.");
    file.seekg(0, std::ios::end);
    std::vector<unsigned char> ttbuffer(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char*>(ttbuffer.data()), ttbuffer.size());
    std::vector<unsigned char> atlas_data(gearoenix::render::font::Font2D::baked_aspect * gearoenix::render::font::Font2D::baked_aspect);
    stbtt_BakeFontBitmap(
        ttbuffer.data(), 0,
        (float)gearoenix::render::font::Font2D::baked_aspect / (float)root_nchar,
        atlas_data.data(), gearoenix::render::font::Font2D::baked_aspect, gearoenix::render::font::Font2D::baked_aspect,
        gearoenix::render::font::Font2D::first_character, gearoenix::render::font::Font2D::characters_count,
        bkchar.data());
    for (int i = 0; i < gearoenix::render::font::Font2D::characters_count; ++i) {
        float xx = 0.0f, yy = 0.0f;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(bkchar.data(), gearoenix::render::font::Font2D::baked_aspect, gearoenix::render::font::Font2D::baked_aspect, i, &xx, &yy, &q, 1);
        gearoenix::render::font::Font2D::LetterProperties lp;
        lp.pos_max = gearoenix::math::Vec2(q.x1, q.y1);
        lp.pos_min = gearoenix::math::Vec2(q.x0, q.y0);
        lp.uv_max = gearoenix::math::Vec2(q.s1, q.t1);
        lp.uv_min = gearoenix::math::Vec2(q.s0, q.t0);
        lps[i] = lp;
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    gearoenix::core::Real min_y = std::numeric_limits<gearoenix::core::Real>::max();
    gearoenix::core::Real max_y = -min_y;
    for (int lpi = 0; lpi < gearoenix::render::font::Font2D::characters_count; ++lpi) {
        const gearoenix::core::Real l_max_y = lps[lpi].pos_max[1];
        const gearoenix::core::Real l_min_y = lps[lpi].pos_min[1];
        if (l_max_y > max_y)
            max_y = l_max_y;
        if (l_min_y < min_y)
            min_y = l_min_y;
    }
    const gearoenix::core::Real line_gap = max_y - min_y;
    const gearoenix::core::Real scale = 1.0f / line_gap;
    for (int lpi = 0; lpi < gearoenix::render::font::Font2D::characters_count; ++lpi) {
        auto& lp = lps[lpi];
        lp.pos_max[1] -= max_y;
        lp.pos_min[1] -= max_y;
        const gearoenix::core::Real tran_x = 0.0f - lp.pos_min[0];
        lp.pos_min[0] = 0.0f;
        lp.pos_max[0] += tran_x;
        lp.pos_max *= scale;
        lp.pos_min *= scale;
    }
    auto mid_placer = [&](int lpi) -> void {
        auto& lp = lps[lpi];
        const gearoenix::core::Real h = lp.pos_max[1] - lp.pos_min[1];
        const gearoenix::core::Real pad_y = (1.0f - h) * -0.5f;
        const gearoenix::core::Real trn_y = pad_y - lp.pos_max[1];
        lp.pos_max[1] = pad_y;
        lp.pos_min[1] += trn_y;
    };
    auto btm_placer = [&](int lpi, gearoenix::core::Real btm) -> void {
        auto& lp = lps[lpi];
        const gearoenix::core::Real trn_y = btm - lp.pos_min[1];
        lp.pos_min[1] = btm;
        lp.pos_max[1] += trn_y;
    };
    auto top_placer = [&](int lpi, gearoenix::core::Real top) -> void {
        auto& lp = lps[lpi];
        const gearoenix::core::Real trn_y = top - lp.pos_max[1];
        lp.pos_max[1] = top;
        lp.pos_min[1] += trn_y;
    };
    const int cap_a_lpi = ((int)'A') - gearoenix::render::font::Font2D::first_character;
    const int cap_i_lpi = ((int)'I') - gearoenix::render::font::Font2D::first_character;
    const int cap_o_lpi = ((int)'O') - gearoenix::render::font::Font2D::first_character;
    const int lit_a_lpi = ((int)'a') - gearoenix::render::font::Font2D::first_character;
    const int lit_i_lpi = ((int)'i') - gearoenix::render::font::Font2D::first_character;
    const int lit_x_lpi = ((int)'x') - gearoenix::render::font::Font2D::first_character;
    const int dot_lpi = ((int)'.') - gearoenix::render::font::Font2D::first_character;
    const int comma_lpi = ((int)',') - gearoenix::render::font::Font2D::first_character;
    const int semic_lpi = ((int)';') - gearoenix::render::font::Font2D::first_character;
    mid_placer(cap_a_lpi);
    const auto cap_a_bottom = lps[cap_a_lpi].pos_min[1];
    const auto cap_a_top = lps[cap_a_lpi].pos_max[1];
    btm_placer(cap_i_lpi, cap_a_bottom);
    const auto cap_i_top = lps[cap_i_lpi].pos_max[1];
    mid_placer(cap_o_lpi);
    const auto cap_o_top = lps[cap_o_lpi].pos_max[1];
    btm_placer(lit_a_lpi, cap_a_bottom);
    const auto lit_a_top = lps[lit_a_lpi].pos_max[1];
    btm_placer(lit_i_lpi, cap_a_bottom);
    const auto lit_i_top = lps[lit_i_lpi].pos_max[1];
    btm_placer(dot_lpi, cap_a_bottom);
    const auto dot_top = lps[dot_lpi].pos_max[1];
    top_placer(comma_lpi, dot_top);
    const auto comma_bottom = lps[comma_lpi].pos_min[1];
    btm_placer(semic_lpi, comma_bottom);
    const auto semic_top = lps[semic_lpi].pos_max[1];
    btm_placer(lit_x_lpi, cap_a_bottom);
    const auto lit_x_top = lps[lit_x_lpi].pos_max[1];
    for (int lpi = 0, ci = gearoenix::render::font::Font2D::first_character; lpi < gearoenix::render::font::Font2D::characters_count; ++lpi, ++ci) {
        switch ((char)ci) {
        case ' ':
            break;
        case '!':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '"':
            top_placer(lpi, cap_a_top);
            break;
        case '#':
            mid_placer(lpi);
            break;
        case '$':
            mid_placer(lpi);
            break;
        case '%':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '&':
            mid_placer(lpi);
            break;
        case '\'':
            mid_placer(lpi);
            break;
        case '(':
            mid_placer(lpi);
            break;
        case ')':
            mid_placer(lpi);
            break;
        case '*':
            mid_placer(lpi);
            break;
        case '+':
            mid_placer(lpi);
            break;
        case ',':
            break;
        case '-':
            mid_placer(lpi);
            break;
        case '.':
            break;
        case '/':
            mid_placer(lpi);
            break;
        case '0':
            mid_placer(lpi);
            break;
        case '1':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '2':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '3':
            mid_placer(lpi);
            break;
        case '4':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '5':
            mid_placer(lpi);
            break;
        case '6':
            mid_placer(lpi);
            break;
        case '7':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '8':
            mid_placer(lpi);
            break;
        case '9':
            mid_placer(lpi);
            break;
        case ':':
            top_placer(lpi, semic_top);
            break;
        case ';':
            break;
        case '<':
            mid_placer(lpi);
            break;
        case '=':
            mid_placer(lpi);
            break;
        case '>':
            mid_placer(lpi);
            break;
        case '?':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '@':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'A':
            break;
        case 'B':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'C':
            mid_placer(lpi);
            break;
        case 'D':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'E':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'F':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'G':
            mid_placer(lpi);
            break;
        case 'H':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'I':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'J':
            top_placer(lpi, cap_i_top);
            break;
        case 'K':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'L':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'M':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'N':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'O':
            break;
        case 'P':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'Q':
            top_placer(lpi, cap_o_top);
            break;
        case 'R':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'S':
            mid_placer(lpi);
            break;
        case 'T':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'U':
            mid_placer(lpi);
            break;
        case 'V':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'W':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'X':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'Y':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'Z':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '[':
            mid_placer(lpi);
            break;
        case '\\':
            mid_placer(lpi);
            break;
        case ']':
            mid_placer(lpi);
            break;
        case '^':
            top_placer(lpi, cap_a_top);
            break;
        case '_':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '`':
            top_placer(lpi, cap_a_top);
            break;
        case 'a':
            break;
        case 'b':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'c':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'd':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'e':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'f':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'g':
            top_placer(lpi, lit_a_top);
            break;
        case 'h':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'i':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'j':
            top_placer(lpi, lit_i_top);
            break;
        case 'k':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'l':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'm':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'n':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'o':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'p':
            top_placer(lpi, lit_a_top);
            break;
        case 'q':
            top_placer(lpi, lit_a_top);
            break;
        case 'r':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 's':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 't':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'u':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'v':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'w':
            btm_placer(lpi, cap_a_bottom);
            break;
        case 'x':
            break;
        case 'y':
            top_placer(lpi, lit_x_top);
            break;
        case 'z':
            btm_placer(lpi, cap_a_bottom);
            break;
        case '{':
            mid_placer(lpi);
            break;
        case '|':
            mid_placer(lpi);
            break;
        case '}':
            mid_placer(lpi);
            break;
        case '~':
            mid_placer(lpi);
            break;
        default:
            mid_placer(lpi);
            break;
        }
    }
    std::ofstream baked_file(argv[2], std::ios::binary | std::ios::out);
    baked_file.write(reinterpret_cast<const char*>(lps.data()), sizeof(lps[0]) * lps.size());
    gearoenix::core::Count png_size = 0;
    const auto png_size_pos = baked_file.tellp();
    baked_file.write(reinterpret_cast<const char*>(&png_size), sizeof(png_size));
    stbi_write_png_to_func(
        png_write_function,
        &baked_file,
        gearoenix::render::font::Font2D::baked_aspect,
        gearoenix::render::font::Font2D::baked_aspect,
        1, atlas_data.data(), 0);
    png_size = baked_file.tellp() - png_size_pos;
    baked_file.seekp(png_size_pos);
    baked_file.write(reinterpret_cast<const char*>(&png_size), sizeof(png_size));
    return 0;
}
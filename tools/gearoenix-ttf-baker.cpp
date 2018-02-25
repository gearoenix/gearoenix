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
    //        core::Real cap_a_base = letters_properties[((int)'A') - FIRST_CHAR].pos_min[1];
    for (int lpi = 0, ci = gearoenix::render::font::Font2D::first_character; lpi < gearoenix::render::font::Font2D::characters_count; ++lpi, ++ci) {
        mid_placer(lpi);
    }

    std::ofstream baked_file(argv[2], std::ios::binary | std::ios::out);
    baked_file.write(reinterpret_cast<const char*>(lps.data()), sizeof(lps[0]) * lps.size());
    stbi_write_png_to_func(
        png_write_function,
        &baked_file,
        gearoenix::render::font::Font2D::baked_aspect,
        gearoenix::render::font::Font2D::baked_aspect,
        1, atlas_data.data(), 0);

    return 0;
}
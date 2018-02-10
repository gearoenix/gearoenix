#define STB_TRUETYPE_IMPLEMENTATION
#include "rnd-txt-lodepng.hpp"
#include "stb_truetype.h"
#include <iostream>

int main()
{
    const int first_char = 33, num_chars = 94;
    stbtt_bakedchar* bkchar = new stbtt_bakedchar[960];
    unsigned char* ttbuffer = new unsigned char[24 << 20];
    FILE* fd = fopen("font.ttf", "rb");
    if (fd == NULL) {
        std::cerr << "File not found." << std::endl;
        std::exit(-1);
    }
    fread(ttbuffer, 1, 24 << 20, fd);
    unsigned char* atlas_data = new unsigned char[1 << 20];
    stbtt_BakeFontBitmap(
        ttbuffer, 0, // font location (use offset=0 for plain .ttf)
        100, // height of font in pixels
        atlas_data, 1024, 1024, // bitmap to be filled in
        first_char, num_chars, // characters to bake
        bkchar);
    unsigned char* imgdata = new unsigned char[4 << 20];
    for (int i = 0, j = 0; i < 4 << 20; ++i, ++j) {
        if (atlas_data[j] != 0) {
            imgdata[i] = 0XFF;
            imgdata[++i] = 0XFF;
            imgdata[++i] = 0XFF;
            imgdata[++i] = (unsigned char)((((int)0XFF) * ((int)atlas_data[j])) / ((int)0xFF));
        } else {
            imgdata[i] = 0X0;
            imgdata[++i] = 0X0;
            imgdata[++i] = 0X0;
            imgdata[++i] = 0X0;
        }
    }
    float max_w = 0.0f, max_h = 0.0f;
    for (int i = 0; i < num_chars; ++i) {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, 1024, 1024, i, &xx, &yy, &q, 1);
        const float cur_w = q.s1 - q.s0;
        const float cur_h = q.t1 - q.t0;
        if (cur_w > max_w)
            max_w = cur_w;
        if (cur_h > max_h)
            max_h = cur_h;
    }
    const float max_a = max_h > max_w ? max_h : max_w;
    const float rootnchar = std::ceil(std::sqrt((float)num_chars));
    const float slot_a = 1.0f / rootnchar;
    const float scale = 1.0f / ((rootnchar + 1.0f) * max_a);
    auto place_finder = [bkchar, slot_a, scale](int index, float& w, float& h, float& pw, float& ph) -> void {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, 1024, 1024, i, &xx, &yy, &q, 1);
        w = (q.s1 - q.s0) * scale;
        h = (q.t1 - q.t0) * scale;
        pw = (slot_a - w) * 0.5f;
        ph = (slot_a - h) * 0.5f;
    };

    for (int i = 0; i < num_chars; ++i) {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, 1024, 1024, i, &xx, &yy, &q, 1);
        const float w = (q.s1 - q.s0) * scale;
        const float h = (q.t1 - q.t0) * scale;
        const float pw = (slot_a - w) * 0.5f;
        const float ph = (slot_a - h) * 0.5f;
    }

    for (int i = 0; i < num_chars; ++i) {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, 1024, 1024, i, &xx, &yy, &q, 1); //1=opengl & d3d10+,0=d3d9
        const int qt0 = (int)(q.t0 * 1024.0f);
        const int qt1 = (int)(q.t1 * 1024.0f);
        const int qs0 = (int)(q.s0 * 1024.0f);
        const int qs1 = (int)(q.s1 * 1024.0f);
        for (int y = qt0; y < qt1; ++y) {
            int indx = (qs0 + y * 1024) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
            indx = (qs1 + y * 1024) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
        }
        for (int x = qs0; x < qs1; ++x) {
            int indx = (x + qt0 * 1024) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
            indx = (x + qt1 * 1024) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
        }
    }
    lodepng::encode("out.png", imgdata, 1024, 1024);
    return 0;
}
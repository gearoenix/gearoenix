#include "../gearoenix/render/texture/rnd-txt-lodepng.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../external/stb/stb_truetype.h"
#include <cmath>
#include <cstring>
#include <iostream>

#define ATLAS_ASPECT 512
#define MAIN_ASPECT 1024

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
        180, // height of font in pixels
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
    std::cout << "max width: " << max_w << ", max height: " << max_h << std::endl;
    unsigned char* final_image = new unsigned char[ATLAS_ASPECT * ATLAS_ASPECT * 4];
    std::memset(final_image, 0, ATLAS_ASPECT * ATLAS_ASPECT * 4);
    const float max_a = max_h > max_w ? max_h : max_w;
    const float rootnchar = std::ceil(std::sqrt((float)num_chars));
    const float slot_a = 1.0f / rootnchar;
    const float iscale = (rootnchar + 1.0f) * max_a;
    const float scale = 1.0f / iscale;
    auto place_finder = [bkchar, slot_a, scale](int index, float& w, float& h, float& pw, float& ph, float& x0, float& y0) -> void {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, 1024, 1024, index, &xx, &yy, &q, 1);
        w = (q.s1 - q.s0) * scale;
        h = (q.t1 - q.t0) * scale;
        pw = (slot_a - w) * 0.5f;
        ph = (slot_a - h) * 0.5f;
        x0 = q.s0 * (float)MAIN_ASPECT;
        y0 = q.t0 * (float)MAIN_ASPECT;
    };
    float tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    place_finder(((int)'A') - first_char, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
    const float charApadv = tmp4;
    for (int i = 0; i < num_chars; ++i) {
        float w, h, pw, ph, x0, y0;
        place_finder(i, w, h, pw, ph, x0, y0);
        const char curchar = (char)(num_chars + first_char);
        const float row = slot_a * ((float)i);
        float tmp;
        const float x = std::modf(row, &tmp) + pw;
        const float y = slot_a * tmp + ph;
        const int pxw = (int)std::round(((float)ATLAS_ASPECT) * w);
        const int pxh = (int)std::round(((float)ATLAS_ASPECT) * h);
        const int pxxi = (int)std::round(((float)ATLAS_ASPECT) * x);
        const int pxyi = (int)std::round(((float)ATLAS_ASPECT) * y);
        for (int xi = 0; xi < pxw; ++xi) {
            for (int yi = 0; yi < pxh; ++yi) {
                const int atspxxi = (int)std::round(((float)xi) * iscale * ((float)MAIN_ASPECT / (float)ATLAS_ASPECT) + x0);
                const int atspxyi = (int)std::round(((float)yi) * iscale * ((float)MAIN_ASPECT / (float)ATLAS_ASPECT) + y0);
                const int atspxi = atspxyi * MAIN_ASPECT + atspxxi;
                const int pxi = (pxxi + xi + (pxyi + yi) * ATLAS_ASPECT) << 2;
                if (atlas_data[atspxi] == 0) {
                    final_image[pxi] = 0;
                    final_image[pxi + 1] = 0;
                    final_image[pxi + 2] = 0;
                    final_image[pxi + 3] = 0;
                } else {
                    final_image[pxi] = 0XFF;
                    final_image[pxi + 1] = 0XFF;
                    final_image[pxi + 2] = 0XFF;
                    final_image[pxi + 3] = atlas_data[atspxi];
                }
            }
        }
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
    lodepng::encode("stage1.png", imgdata, 1024, 1024);
    lodepng::encode("final.png", final_image, ATLAS_ASPECT, ATLAS_ASPECT);
    return 0;
}
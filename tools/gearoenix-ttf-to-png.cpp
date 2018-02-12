#include "../gearoenix/render/texture/rnd-txt-lodepng.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../external/stb/stb_truetype.h"
#include <cmath>
#include <cstring>
#include <iostream>

#define ATLAS_ASPECT 512
#define MAIN_ASPECT 1024

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage is: gearoenix-ttf-to-png.exe <input ttf file> <output png file>" << std::endl;
        std::exit(-1);
    }
    const int first_char = 33, num_chars = 94;
    stbtt_bakedchar* bkchar = new stbtt_bakedchar[960];
    unsigned char* ttbuffer = new unsigned char[24 << 20];
    FILE* fd = fopen(argv[1], "rb");
    if (fd == NULL) {
        std::cerr << "File not found." << std::endl;
        std::exit(-1);
    }
    fread(ttbuffer, 1, 24 << 20, fd);
    unsigned char* atlas_data = new unsigned char[1 << 20];
    stbtt_BakeFontBitmap(
        ttbuffer, 0, // font location (use offset=0 for plain .ttf)
        160, // height of font in pixels
        atlas_data, MAIN_ASPECT, MAIN_ASPECT, // bitmap to be filled in
        first_char, num_chars, // characters to bake
        bkchar);
#ifdef DEBUG_MODE_FONT
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
#endif
    float max_w = 0.0f, max_h = 0.0f;
    for (int i = 0; i < num_chars; ++i) {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, MAIN_ASPECT, MAIN_ASPECT, i, &xx, &yy, &q, 1);
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
    const float iscale = (rootnchar + 2.0f) * max_a;
    const float scale = 1.0f / iscale;
    auto place_finder = [bkchar, slot_a, scale](int index, float& w, float& h, float& pw, float& ph, float& x0, float& y0) -> void {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, MAIN_ASPECT, MAIN_ASPECT, index, &xx, &yy, &q, 1);
        w = (q.s1 - q.s0) * scale;
        h = (q.t1 - q.t0) * scale;
        pw = (slot_a - w) * 0.5f;
        ph = (slot_a - h) * 0.5f;
        x0 = q.s0 * (float)MAIN_ASPECT;
        y0 = q.t0 * (float)MAIN_ASPECT;
    };
    float tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
    place_finder(((int)'A') - first_char, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
    const float char_cap_a_padv = tmp4;
    place_finder(((int)'.') - first_char, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
    const float char_dot_padv = slot_a - (tmp2 + char_cap_a_padv);
    place_finder(((int)':') - first_char, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
    const float char_dcom_padv = slot_a - (tmp2 + char_cap_a_padv);
    place_finder(((int)'a') - first_char, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
    const float char_lit_a_padv = slot_a - (tmp2 + char_cap_a_padv);
    place_finder(((int)'i') - first_char, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
    const float char_lit_i_padv = slot_a - (tmp2 + char_cap_a_padv);
    for (int i = 0; i < num_chars; ++i) {
        float w, h, pw, ph, x0, y0;
        place_finder(i, w, h, pw, ph, x0, y0);
        const char curchar = (char)(i + first_char);
        switch (curchar) {
        case '!':
            ph = slot_a - (h + char_cap_a_padv);
            break;
        case '"':
        case '\'':
        case '`':
        case 'Q':
            ph = char_cap_a_padv;
            break;
        case '.':
        case ',':
            ph = char_dot_padv;
            break;
        case ':':
        case ';':
            ph = char_dcom_padv;
            break;
        case 'i':
        case 'j':
            ph = char_lit_i_padv;
            break;
        case 'b':
        case 'd':
        case 'f':
        case 'h':
        case 'k':
        case 'l':
        case 't':
            ph = slot_a - (h + char_cap_a_padv);
            break;
        default:
            if ('a' <= curchar && curchar <= 'z')
                ph = char_lit_a_padv;
            break;
        }
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
#ifdef DEBUG_MODE_FONT
    for (int i = 0, pxi = 0; i < (int)rootnchar; ++i, pxi = (i * ATLAS_ASPECT * (ATLAS_ASPECT / ((int)rootnchar))) << 2)
        for (int j = 0; j < ATLAS_ASPECT; ++j, ++pxi) {
            final_image[pxi] = 0XFF;
            final_image[++pxi] = 0X00;
            final_image[++pxi] = 0X00;
            final_image[++pxi] = 0XFF;
        }
    for (int i = 0; i < num_chars; ++i) {
        stbtt_aligned_quad q;
        float xx = 0.0f, yy = 0.0f;
        stbtt_GetBakedQuad(bkchar, MAIN_ASPECT, MAIN_ASPECT, i, &xx, &yy, &q, 1); //1=opengl & d3d10+,0=d3d9
        const int qt0 = (int)(q.t0 * ((float)MAIN_ASPECT));
        const int qt1 = (int)(q.t1 * ((float)MAIN_ASPECT));
        const int qs0 = (int)(q.s0 * ((float)MAIN_ASPECT));
        const int qs1 = (int)(q.s1 * ((float)MAIN_ASPECT));
        for (int y = qt0; y < qt1; ++y) {
            int indx = (qs0 + y * MAIN_ASPECT) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
            indx = (qs1 + y * MAIN_ASPECT) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
        }
        for (int x = qs0; x < qs1; ++x) {
            int indx = (x + qt0 * MAIN_ASPECT) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
            indx = (x + qt1 * MAIN_ASPECT) << 2;
            imgdata[indx] = 0XFF;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0X00;
            imgdata[++indx] = 0XFF;
        }
    }
    lodepng::encode("stage1.png", imgdata, MAIN_ASPECT, MAIN_ASPECT);
#endif
    lodepng::encode(argv[2], final_image, ATLAS_ASPECT, ATLAS_ASPECT);
    return 0;
}
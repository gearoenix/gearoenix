#include "rnd-txt-png.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include <png.h>

#define GEAROENIX_PNG_SIGNATURE_SIZE 8

#ifdef DEBUG_MODE
#define GEAROENIX_PNG_DEC_DEBUG
#endif

static void png_data_reader(png_structp png_ptr, png_bytep data, png_size_t length)
{
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == nullptr)
        LOGF("Unexpected");
    gearoenix::system::File* f = reinterpret_cast<gearoenix::system::File*>(io_ptr);
    f->read(data, length);
}

static bool validate(gearoenix::system::File* file)
{
    png_byte pngsig[GEAROENIX_PNG_SIGNATURE_SIZE];
    int is_png = 0;
    file->read(pngsig, GEAROENIX_PNG_SIGNATURE_SIZE);
    is_png = png_sig_cmp(pngsig, 0, GEAROENIX_PNG_SIGNATURE_SIZE);
    return (is_png == 0);
}

void gearoenix::render::texture::PNG::decode(
    system::File* file,
    Engine*,
    std::vector<unsigned char>& data,
    unsigned int& img_width,
    unsigned int& img_height,
    unsigned int& channels)
{
    LOGI("PNG file size is: " << file->read<core::Count>());
    if (!validate(file)) {
        LOGF("Png is not validated.");
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        LOGF("Png initialization error.");
    }
    png_set_read_fn(png_ptr, (png_voidp)file, png_data_reader);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
        LOGF("Png information error.");
    }
    png_bytep* row_ptrs = NULL;
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
        if (row_ptrs != NULL)
            delete[] row_ptrs;
        LOGF("Png parsing error.");
    }
    png_set_sig_bytes(png_ptr, GEAROENIX_PNG_SIGNATURE_SIZE);
    png_read_info(png_ptr, info_ptr);
    img_width = png_get_image_width(png_ptr, info_ptr);
    img_height = png_get_image_height(png_ptr, info_ptr);
    png_uint_32 bitdepth = png_get_bit_depth(png_ptr, info_ptr);
    channels = png_get_channels(png_ptr, info_ptr);
    png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);
#ifdef DEBUG_MODE
    LOGI("Image width is %d." << img_height);
    LOGI("Image height is %d." << img_width);
    LOGI("Image bit depth is %d." << (int)bitdepth);
    LOGI("Image channels is %d." << channels);
    LOGI("Image color type is %d." << (int)color_type);
#endif
    switch (color_type) {
    case PNG_COLOR_TYPE_GA:
        if (bitdepth < 8)
            png_set_expand_gray_1_2_4_to_8(png_ptr);
        else
            png_set_gray_to_rgb(png_ptr);
        break;
    case PNG_COLOR_TYPE_GRAY:
        if (bitdepth < 8)
            png_set_expand_gray_1_2_4_to_8(png_ptr);
        else
            png_set_gray_to_rgb(png_ptr);
        break;
    case PNG_COLOR_TYPE_PALETTE:
        png_set_palette_to_rgb(png_ptr);
        break;
    default:
        break;
    }
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        channels += 1;
    }
    row_ptrs = new png_bytep[img_height];
    data.resize(img_width * img_height * bitdepth * channels / 8);
    const unsigned int stride = img_width * bitdepth * channels / 8;
    for (size_t i = 0; i < img_height; i++) {
        png_uint_32 q = png_uint_32((img_height - i - 1) * stride);
        row_ptrs[i] = (png_bytep)data.data() + q;
    }
    png_read_image(png_ptr, row_ptrs);
    delete[](png_bytep) row_ptrs;
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
}

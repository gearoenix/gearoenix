#include "rnd-txt-png.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#include "../../../external/stb/stb_image.h"
#include <cstring>

#define GEAROENIX_PNG_SIGNATURE_SIZE 8

#ifdef GX_DEBUG_MODE
#define GEAROENIX_PNG_DEC_DEBUG
#endif

void gearoenix::render::texture::PNG::decode(
    system::stream::Stream* file,
    std::vector<unsigned char>& data,
    unsigned int& img_width,
    unsigned int& img_height)
{
    std::vector<unsigned char> png;
    file->read(png);
    int iw, ih, channels;
    unsigned char* dd = stbi_load_from_memory(png.data(), (int)png.size(), &iw, &ih, &channels, 4);
    if (dd == nullptr) {
        GXLOGF("PNG decoder error.");
    }
    img_width = (unsigned int)iw;
    img_height = (unsigned int)ih;
    const unsigned int img_size = img_width * img_height * 4;
    data.resize(img_size);
    std::memcpy(&(data[0]), dd, img_size);
    stbi_image_free(dd);
}

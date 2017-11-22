#include "rnd-txt-png.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "rnd-txt-lodepng.hpp"

#define GEAROENIX_PNG_SIGNATURE_SIZE 8

#ifdef DEBUG_MODE
#define GEAROENIX_PNG_DEC_DEBUG
#endif

void gearoenix::render::texture::PNG::decode(
    system::File* file,
    std::vector<unsigned char>& data,
    unsigned int& img_width,
    unsigned int& img_height)
{
    std::vector<unsigned char> png;
    file->read(png);
    unsigned error = lodepng::decode(data, img_width, img_height, png);
    if (error) {
        GXLOGF("decoder error " << error << ": " << lodepng_error_text(error));
    }
}

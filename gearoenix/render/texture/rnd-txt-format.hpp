#ifndef GEAROENIX_RENDER_TEXTURE_FORMAT_HPP
#define GEAROENIX_RENDER_TEXTURE_FORMAT_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::texture {
class TextureFormat {
public:
    typedef enum : core::TypeId {
        RGBA_FLOAT16 = 1,
        RGB_FLOAT16 = 2,
        RG_FLOAT16 = 3,
        R_FLOAT16 = 4,

        RGBA_FLOAT24 = 5,
        RGB_FLOAT24 = 6,
        RG_FLOAT24 = 7,
        R_FLOAT24 = 8,

        RGBA_FLOAT32 = 9,
        RGB_FLOAT32 = 10,
        RG_FLOAT32 = 11,
        R_FLOAT32 = 12,

        RGBA_UINT8 = 13,
        RGB_UINT8 = 14,
        RG_UINT8 = 15,
        R_UINT8 = 16,

        D_16 = 17,
        D_32 = 18,
    } Id;
};
}
#endif

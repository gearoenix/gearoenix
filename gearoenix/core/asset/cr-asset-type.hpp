#ifndef GEAROENIX_CORE_ASSET_TYPE_HPP
#define GEAROENIX_CORE_ASSET_TYPE_HPP
namespace gearoenix::core::asset {
class Type {
public:
    typedef enum {
        AUDIO = 1,
        BODY = 2,
        CAMERA = 3,
        CONSTRAINT = 4,
        FONT = 5,
        LIGHT = 6,
        MESH = 7,
        MODEL = 8,
        SCENE = 9,
        SHADER = 10,
        SKYBOX = 11,
        TEXTURE = 12,
        NODE = 13,
    } Id;
};
} // namespace gearoenix
#endif
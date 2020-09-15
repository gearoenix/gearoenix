#ifndef GEAROENIX_CORE_ASSET_TYPE_HPP
#define GEAROENIX_CORE_ASSET_TYPE_HPP
namespace gearoenix::core::asset {
enum struct Type : int {
    Audio = 1,
    Body = 2,
    Camera = 3,
    Constraint = 4,
    Font = 5,
    Light = 6,
    Mesh = 7,
    Model = 8,
    Scene = 9,
    Shader = 10,
    Skybox = 11,
    Texture = 12,
    Reflection = 13,
    Animation = 14,
    Node = 128,
};
}
#endif
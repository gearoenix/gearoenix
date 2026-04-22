#ifndef GEAROENIX_RENDER_CAMERA_UNIFORM_HPP // no pragma, because of some of the shader compilers.
#define GEAROENIX_RENDER_CAMERA_UNIFORM_HPP

#include "../buffer/gx-rnd-buf-uniform-struct.hpp"

GX_UNIFORM_STRUCT_DEF(Camera, {
    mat4 view_projection;
    vec4 position_far;
});

GX_UNIFORM_STRUCT_DEF(CameraJointModel, {
    mat4 mvp;
});

#endif
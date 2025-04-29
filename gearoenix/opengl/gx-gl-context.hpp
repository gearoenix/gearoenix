#pragma once
#include "gx-gl-types.hpp"
#include "../math/gx-math-vector-4d.hpp"

namespace gearoenix::gl::ctx
{
    void set_framebuffer(uint framebuffer_object);
    void set_viewport_scissor_clip(const math::Vec4<sizei>& viewport_clip);
}
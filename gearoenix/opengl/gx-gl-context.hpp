#pragma once
#include "../math/gx-math-vector-4d.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl::ctx {
void set_framebuffer(uint framebuffer_object);
void set_viewport_scissor_clip(const math::Vec4<sizei>& viewport_clip);
}
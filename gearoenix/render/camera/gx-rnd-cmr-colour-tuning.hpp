#pragma once
#include "../../math/gx-math-vector-3d.hpp"
#include <variant>

namespace gearoenix::render::camera {
struct GammaCorrection final {
    math::Vec3<float> gamma_exponent = math::Vec3(1.3f / 2.2f);
};

struct Multiply final {
    math::Vec3<float> scale = math::Vec3(1.0f);
};

struct RawColour final { };

typedef std::variant<GammaCorrection, Multiply, RawColour> ColourTuning;
}
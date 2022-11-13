#ifndef GEAROENIX_RENDER_CAMERA_COLOUR_TUNING_HPP
#define GEAROENIX_RENDER_CAMERA_COLOUR_TUNING_HPP
#include "../../math/gx-math-vector-3d.hpp"
#include <variant>

namespace gearoenix::render::camera {
struct HdrTuneMappingGammaCorrection final {
    float hdr_tune_mapping = 1.0f;
    float gamma_correction = 2.2f;
};

struct Multiply final {
    math::Vec3<float> scale = math::Vec3<float>(1.0f);
};

struct RawColour final { };

typedef std::variant<HdrTuneMappingGammaCorrection, Multiply, RawColour> ColourTuning;
}

#endif
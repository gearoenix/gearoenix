#ifndef GEAROENIX_RENDER_MATERIAL_UNIFORM_HPP
#define GEAROENIX_RENDER_MATERIAL_UNIFORM_HPP
#include "../../core/cr-types.hpp"

namespace gearoenix {
namespace render {
    namespace material {

        struct Uniform {
            core::Real alpha = 1.0f;
            core::Real alpha_cutoff = 0.001f;
            core::Real metallic_factor = 1.0f;
            core::Real normal_scale = 1.0f;
            core::Real occlusion_strength = 1.0f;
            core::Real roughness_factor = 1.0f;
        };

    } // namespace material
} // namespace render
} // namespace gearoenix
#endif

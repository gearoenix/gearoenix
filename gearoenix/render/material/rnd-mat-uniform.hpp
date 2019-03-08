#ifndef GEAROENIX_RENDER_MATERIAL_UNIFORM_HPP
#define GEAROENIX_RENDER_MATERIAL_UNIFORM_HPP
#include "../../core/cr-types.hpp"

namespace gearoenix {
namespace render {
    namespace material {

        struct Uniform {
            core::Real alpha = 1.0;
            core::Real alpha_cutoff = 0.001;
            core::Real metallic_factor = 1.0;
            core::Real normal_scale = 1.0;
            core::Real occlusion_strength = 1.0;
            core::Real roughness_factor = 1.0;
        };

    } // namespace material
} // namespace render
} // namespace gearoenix
#endif

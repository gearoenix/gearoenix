#ifndef GEAROENIX_RENDER_MATERIAL_TRANSLUCENCY_MODE_HPP
#define GEAROENIX_RENDER_MATERIAL_TRANSLUCENCY_MODE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    namespace material {
        class TranslucencyMode {
        public:
            typedef enum : core::Id {
                Cutoff = 1,
                Tansparent = 2,
                Opaque = 3,
            } Id;
        };
    } // namespace material
} // namespace render
} // namespace gearoenix
#endif

#ifndef GEAROEMIX_RENDER_LIGHT_TYPE_HPP
#define GEAROEMIX_RENDER_LIGHT_TYPE_HPP
#include "../../core/cr-types.hpp"

namespace gearoenix {
namespace render {
    namespace light {
        class Type {
        public:
            typedef enum : core::TypeId {
                CONE = 1,
                DIRECTIONAL = 2,
                POINT = 3,
            } Id;
        };
    }
}
}
#endif

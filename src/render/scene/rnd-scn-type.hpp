#ifndef GEAROEMIX_RENDER_SCENE_TYPE_HPP
#define GEAROEMIX_RENDER_SCENE_TYPE_HPP
#include "../../core/cr-types.hpp"

namespace gearoenix {
namespace render {
    namespace scene {
        class Type {
        public:
            typedef enum : core::TypeId {
                GAME = 1,
                UI = 2,
            } Id;
        };
    }
}
}
#endif

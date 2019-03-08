#ifndef GEAROENIX_RENDER_MESH_TYPE_HPP
#define GEAROENIX_RENDER_MESH_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    namespace mesh {
        class Type {
        public:
            typedef enum : core::Id {
                BASIC = 1,
            } Id;
        };

    }
}
}
#endif

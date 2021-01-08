#ifndef GEAROENIX_RENDER_FONT_TYPE_HPP
#define GEAROENIX_RENDER_FONT_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
namespace gearoenix {
namespace render {
    struct Engine;
    namespace font {
        struct Type {
        public:
            typedef enum : core::TypeId {
                D2 = 1,
                D3 = 2,
            } Id;
        };
    }
}
}
#endif

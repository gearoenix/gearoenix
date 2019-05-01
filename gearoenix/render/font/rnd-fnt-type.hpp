#ifndef GEAROEMIX_RENDER_FONT_TYPE_HPP
#define GEAROEMIX_RENDER_FONT_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    class Engine;
    namespace font {
        class Type {
        public:
            typedef enum : core::Id {
                D2 = 1,
                D3 = 2,
            } Id;
        };
    }
}
}
#endif

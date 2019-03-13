#ifndef GEAROEMIX_RENDER_MODEL_TYPE_HPP
#define GEAROEMIX_RENDER_MODEL_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    namespace model {
        class Type {
        public:
			typedef enum : core::TypeId {
				DYNAMIC = 1,
				STATIC = 2,
				WIDGET = 3,
			} Id;
        };
    }
}
}
#endif

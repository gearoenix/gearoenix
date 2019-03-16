#ifndef GEAROEMIX_RENDER_TEXTURE_FORMAT_HPP
#define GEAROEMIX_RENDER_TEXTURE_FORMAT_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    namespace texture {
        class Format {
        public:
			typedef enum : core::TypeId 
			{
				RGBA_FLOAT16 = 1,
				RGBA_FLOAT24 = 2,
				RGBA_FLOAT32 = 3,
				RGBA_UINT8 = 4,
			} Id;
        };
    }
}
}
#endif

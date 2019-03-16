#ifndef GEAROEMIX_RENDER_TEXTURE_SAMPLE_HPP
#define GEAROEMIX_RENDER_TEXTURE_SAMPLE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    namespace texture {
        class Sample {
        public:
			typedef enum : core::TypeId 
			{
				CUBIC = 1,
				LINEAR = 2,
				NEAREST = 3,
			} Id;
        };
    }
}
}
#endif

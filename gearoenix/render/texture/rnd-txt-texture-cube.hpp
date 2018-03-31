#ifndef GEAROEMIX_RENDER_TEXTURE_CUBE_HPP
#define GEAROEMIX_RENDER_TEXTURE_CUBE_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix {
namespace render {
    namespace texture {
        class Cube : public Texture {
        protected:
            Cube(core::Id my_id, Engine* e);

        public:
            virtual ~Cube();
        };
    }
}
}
#endif

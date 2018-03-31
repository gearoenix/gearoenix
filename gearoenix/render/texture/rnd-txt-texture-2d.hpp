#ifndef GEAROEMIX_RENDER_TEXTURE_TEXTURE2D_HPP
#define GEAROEMIX_RENDER_TEXTURE_TEXTURE2D_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix {
namespace render {
    namespace texture {
        class Texture2D : public Texture {
        protected:
            Texture2D(core::Id my_id, Engine* e);

        public:
            virtual ~Texture2D();
        };
    }
}
}
#endif

#ifndef GEAROEMIX_RENDER_TEXTURE_TEXTURE2D_HPP
#define GEAROEMIX_RENDER_TEXTURE_TEXTURE2D_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace texture {
        class TextureCube : public Texture {
        public:
            TextureCube(system::File* file, Engine* engine);
        };
    }
}
}
#endif

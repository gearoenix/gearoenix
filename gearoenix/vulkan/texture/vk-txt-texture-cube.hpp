#ifndef GEAROEMIX_VULKAN_TEXTURE_CUBE_HPP
#define GEAROEMIX_VULKAN_TEXTURE_CUBE_HPP
#include "../../render/texture/rnd-txt-texture.hpp"
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

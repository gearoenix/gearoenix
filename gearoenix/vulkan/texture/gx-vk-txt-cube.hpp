#ifndef GEAROENIX_VULKAN_TEXTURE_CUBE_HPP
#define GEAROENIX_VULKAN_TEXTURE_CUBE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../render/texture/gx-rnd-txt-texture.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace texture {
        class TextureCube : public Texture {
        public:
            TextureCube(system::stream::Stream* file, Engine* engine);
        };
    }
}
}
#endif
#endif

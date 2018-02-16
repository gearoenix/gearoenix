#ifndef GEAROEMIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROEMIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-end-caller.hpp"
#include <memory>
namespace gearoenix {
namespace system {
namespace file {
    class File;
}
}
namespace render {
    class Engine;
    namespace texture {
        class Texture : public core::asset::Asset {
        public:
            virtual ~Texture();
            static Texture* read(system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
        };
    }
}
}
#endif

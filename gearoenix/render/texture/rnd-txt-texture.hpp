#ifndef GEAROEMIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROEMIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
#include <memory>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace texture {
        class Texture : public core::asset::Asset {
        public:
            virtual ~Texture();
            static Texture* read(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
        };
    }
}
}
#endif

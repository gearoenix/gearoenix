#ifndef GEAROEMIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROEMIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    class Engine;
    namespace texture {
        class Texture : public core::asset::Asset {
        protected:
            Texture(core::Id my_id);

        public:
            virtual ~Texture();
            static Texture* read(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
        };
    }
}
}
#endif

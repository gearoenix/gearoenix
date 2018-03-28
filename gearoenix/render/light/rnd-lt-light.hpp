#ifndef GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/asset/cr-asset.hpp"
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    class Engine;
    namespace light {
        class Light : public core::asset::Asset {
        protected:
            Light(core::Id my_id, system::stream::Stream* f, Engine* e);

        public:
            virtual ~Light();
            static Light* read(core::Id my_id, system::stream::Stream* f, Engine* e);
        };
    }
}
}
#endif

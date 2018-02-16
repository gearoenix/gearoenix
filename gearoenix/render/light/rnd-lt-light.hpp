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
    namespace light {
        class Light : public core::asset::Asset {
        protected:
            Light(system::stream::Stream* f);

        public:
            virtual ~Light();
            static Light* read(system::stream::Stream* f);
        };
    }
}
}
#endif

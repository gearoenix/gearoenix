#ifndef GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/asset/cr-asset.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    namespace light {
        class Light : public core::asset::Asset {
        protected:
            Light(system::File* f);

        public:
            virtual ~Light();
            static Light* read(system::File* f);
        };
    }
}
}
#endif

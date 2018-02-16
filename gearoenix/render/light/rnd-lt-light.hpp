#ifndef GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/asset/cr-asset.hpp"
namespace gearoenix {
namespace system {
namespace file {
    class File;
}
}
namespace render {
    namespace light {
        class Light : public core::asset::Asset {
        protected:
            Light(system::file::File* f);

        public:
            virtual ~Light();
            static Light* read(system::file::File* f);
        };
    }
}
}
#endif

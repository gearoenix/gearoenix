#ifndef GEAROEMIX_RENDER_MODEL_MODEL_HPP
#define GEAROEMIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace model {
        class Model : public core::asset::Asset {
        protected:
            Model();

        public:
            static Model* read(system::File* f, Engine* e, core::EndCaller* c);
        };
    }
}
}
#endif

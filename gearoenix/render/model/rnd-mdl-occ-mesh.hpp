#ifndef GEAROENIX_RENDER_MODEL_OCC_MESH_HPP
#define GEAROENIX_RENDER_MODEL_OCC_MESH_HPP
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
    namespace model {
        class OccMesh {
        private:
        public:
            OccMesh(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~OccMesh();
        };
    }
}
}
#endif

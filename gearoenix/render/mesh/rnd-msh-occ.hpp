#ifndef GEAROENIX_RENDER_MESH_OCC_HPP
#define GEAROENIX_RENDER_MESH_OCC_HPP
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
    namespace buffer {
        class Mesh;
    }
    namespace mesh {
        class Occ {
        private:
            buffer::Mesh* buf;

        public:
            Occ(system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            ~Occ();
        };
    }
}
}
#endif

#ifndef GEAROENIX_RENDER_MESH_OCC_HPP
#define GEAROENIX_RENDER_MESH_OCC_HPP
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
    namespace buffer {
        class Mesh;
    }
    namespace mesh {
        class Occ {
        private:
            buffer::Mesh* buf;

        public:
            Occ(system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            ~Occ();
        };
    }
}
}
#endif

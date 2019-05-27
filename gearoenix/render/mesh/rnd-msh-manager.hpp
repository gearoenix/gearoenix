#ifndef GEAROENIX_RENDER_MESH_MANAGER_HPP
#define GEAROENIX_RENDER_MESH_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix {
namespace system::stream {
        class Stream;
    }
namespace render {
    namespace engine {
        class Engine;
    }
    namespace mesh {
        class Mesh;
        class Manager {
        private:
            engine::Engine*const e;
            core::cache::File<Mesh> cache;
            std::weak_ptr<Mesh> icosphere;

        public:
            Manager(system::stream::Stream* s, engine::Engine* e) noexcept;
            ~Manager() noexcept = default;
            std::shared_ptr<Mesh> get_gx3d(core::Id id, core::sync::EndCaller<Mesh>& c) noexcept;
            std::shared_ptr<Mesh> create_icosphere(core::sync::EndCaller<Mesh>& c) noexcept;
        };
    }
}
}
#endif

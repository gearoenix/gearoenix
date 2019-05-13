#ifndef GEAROEMIX_RENDER_MESH_MANAGER_HPP
#define GEAROEMIX_RENDER_MESH_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace mesh {
        class Mesh;
        class Manager {
        private:
            const std::shared_ptr<engine::Engine> e;
            core::cache::File<Mesh> cache;
			std::weak_ptr<Mesh> icosphere;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
			std::shared_ptr<Mesh> get_gx3d(const core::Id id, core::sync::EndCaller<Mesh>& c);
			std::shared_ptr<Mesh> create_icosphere(core::sync::EndCaller<Mesh>& c) noexcept;

        };
    }
}
}
#endif

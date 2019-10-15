#ifndef GEAROENIX_RENDER_MESH_MANAGER_HPP
#define GEAROENIX_RENDER_MESH_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-vertex.hpp"
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
            engine::Engine* const e;
            core::cache::File<Mesh> cache;
            std::weak_ptr<Mesh> icosphere;
            std::weak_ptr<Mesh> plate;
            std::weak_ptr<Mesh> cube;

        public:
            Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
            ~Manager() noexcept = default;
            std::shared_ptr<Mesh> get_gx3d(core::Id id, core::sync::EndCaller<Mesh>& c) noexcept;
            std::shared_ptr<Mesh> create_icosphere(core::sync::EndCaller<Mesh>& c) noexcept;
            std::shared_ptr<Mesh> create_plate(core::sync::EndCaller<Mesh>& c) noexcept;
            std::shared_ptr<Mesh> create_cube(core::sync::EndCaller<Mesh>& c) noexcept;
            std::shared_ptr<Mesh> create(const std::vector<math::BasicVertex> &vertices, const std::vector<std::uint32_t> &indices, const math::Aabb3& occlusion_box, core::sync::EndCaller<Mesh>& c) noexcept;
            std::shared_ptr<Mesh> create(const std::vector<math::BasicVertex> &vertices, const std::vector<std::uint32_t> &indices, core::sync::EndCaller<Mesh>& c) noexcept;
        };
    }
}
}
#endif

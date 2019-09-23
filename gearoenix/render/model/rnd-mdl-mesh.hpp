#ifndef GEAROENIX_RENDER_MODEL_MESH_HPP
#define GEAROENIX_RENDER_MODEL_MESH_HPP
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
    namespace material {
        class Material;
    }
    namespace mesh {
        class Mesh;
    }
    namespace model {
        class Mesh {
        protected:
            std::shared_ptr<mesh::Mesh> msh;
            std::shared_ptr<material::Material> mat;

        public:
            Mesh(
                system::stream::Stream* f,
                engine::Engine* e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

            Mesh(
                std::shared_ptr<mesh::Mesh> msh,
                std::shared_ptr<material::Material> mat) noexcept;

            ~Mesh() noexcept;

            [[nodiscard]] const std::shared_ptr<mesh::Mesh>& get_mesh() const noexcept;
            [[nodiscard]] const std::shared_ptr<material::Material>& get_material() const noexcept;
            void update_uniform() noexcept;
        };
    }
}
}
#endif

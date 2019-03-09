#ifndef GEAROEMIX_RENDER_MODEL_MESH_HPP
#define GEAROEMIX_RENDER_MODEL_MESH_HPP
#include "../../core/cr-types.hpp"
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
            const std::shared_ptr<engine::Engine> e;
            core::Real radius = 0.0;
			std::shared_ptr<material::Material> mat;
			std::shared_ptr<mesh::Mesh> msh;

            Mesh();

        public:
            static Mesh* read_gx3d(
                const core::Id mesh_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);

            virtual ~Mesh();

            const std::shared_ptr<mesh::Mesh>& get_mesh() const;
            const std::shared_ptr<material::Material>& get_material() const;
            core::Real get_radius() const;
        };
    }
}
}
#endif

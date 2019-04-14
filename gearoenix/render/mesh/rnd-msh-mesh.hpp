#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-msh-type.hpp"
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
    namespace buffer {
        class Buffer;
    }
    namespace material {
        class Material;
    }
    namespace model {
        class Model;
    }
    namespace scene {
        class Scene;
    }
    namespace texture {
        class Texture2D;
    }
    namespace mesh {
        class Mesh : public core::asset::Asset {
        protected:
            const Type::Id mesh_type_id;

            core::Real radius = 0.0f;
            std::shared_ptr<buffer::Buffer> vertex_buffer = nullptr;
            std::shared_ptr<buffer::Buffer> index_buffer = nullptr;

            Mesh(const core::Id my_id, const Type::Id mesh_type_id);

        public:
            Mesh(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            ~Mesh();
            void draw();
            core::Real get_radius() const;
            Type::Id get_mesh_type_id() const;
            const std::shared_ptr<buffer::Buffer>& get_vertex_buffer() const;
            const std::shared_ptr<buffer::Buffer>& get_index_buffer() const;
        };
    }
}
}
#endif

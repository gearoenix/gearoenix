#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vertex.hpp"
#include "rnd-msh-type.hpp"
namespace gearoenix {
namespace system::stream {
    class Stream;
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
            buffer::Buffer* vertex_buffer = nullptr;
            buffer::Buffer* index_buffer = nullptr;

            Mesh(core::Id my_id, Type::Id mesh_type_id) noexcept;

        public:
            Mesh(
                core::Id my_id,
                system::stream::Stream* f,
                engine::Engine* e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
            Mesh(
                core::Id my_id,
                std::vector<math::BasicVertex> vertices,
                std::vector<std::uint32_t> indices,
                core::Real radius,
                engine::Engine* e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
            ~Mesh() noexcept final;
            void set_vertices(
                engine::Engine* e,
                std::vector<math::BasicVertex> vertices,
                std::vector<std::uint32_t> indices,
                core::Real radius,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
            core::Real get_radius() const noexcept;
            Type::Id get_mesh_type_id() const noexcept;
            const buffer::Buffer* get_vertex_buffer() const noexcept;
            buffer::Buffer* get_vertex_buffer() noexcept;
            const buffer::Buffer* get_index_buffer() const noexcept;
            buffer::Buffer* get_index_buffer() noexcept;
        };
    }
}
}
#endif

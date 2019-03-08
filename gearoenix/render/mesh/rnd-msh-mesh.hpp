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
        class Mesh;
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
            std::shared_ptr<buffer::Mesh> buf;
            const Type::Id t;
            Mesh(
                const Type::Id t,
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore> c);

        public:
            static Mesh* read(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            ~Mesh();
            void bind();
            void draw();
        };
    }
}
}
#endif

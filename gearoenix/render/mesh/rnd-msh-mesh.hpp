#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include <memory>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace system {
    class File;
}
namespace render {
    class Engine;
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
        class Mesh {
        private:
            buffer::Mesh* buf;
            material::Material* mat;

        public:
            Mesh(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~Mesh();
            void commit(const scene::Scene* s, const model::Model* m);
            void draw(texture::Texture2D* shadow_texture);
        };
    }
}
}
#endif

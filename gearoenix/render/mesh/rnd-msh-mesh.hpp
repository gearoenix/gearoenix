#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include <memory>
#include "../../core/asset/cr-asset.hpp"
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
        class Mesh : public core::asset::Asset {
        private:
            buffer::Mesh* buf;

        public:
            Mesh(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~Mesh();
            void bind();
            void draw();
        };
    }
}
}
#endif

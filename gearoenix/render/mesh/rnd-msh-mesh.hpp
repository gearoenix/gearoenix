#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
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
        public:
            class Geo {
            public:
                typedef enum : core::Id {
                    BASIC = 1,
                } Type;
            };

        private:
            buffer::Mesh* buf;

        public:
            Mesh(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            static Mesh* read(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            ~Mesh();
            void bind();
            void draw();
        };
    }
}
}
#endif

#ifndef GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#define GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#include "rnd-mdl-model.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    namespace mesh {
        class Occ;
    }
    namespace texture {
        class Texture2D;
    }
    namespace model {
        class Dynamic : public Model {
        private:
            std::vector<Model*> children;
            mesh::Occ* occmesh;

        public:
            Dynamic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            void commit(const scene::Scene* s);
            void commit(const scene::Scene* s, const Model*);
            void draw(texture::Texture2D* shadow_texture);
        };
    }
}
}
#endif

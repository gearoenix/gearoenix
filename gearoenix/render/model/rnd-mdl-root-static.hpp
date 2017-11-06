#ifndef GEAROENIX_RENDER_MODEL_ROOT_STATIC_HPP
#define GEAROENIX_RENDER_MODEL_ROOT_STATIC_HPP
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
        class RootStatic : public Model {
        private:
            std::vector<Model*> children;
            mesh::Occ* occmesh;

        public:
            RootStatic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~RootStatic();
            void commit(const scene::Scene* s);
            void commit(const scene::Scene* s, const Model* parent);
            void draw(texture::Texture2D* shadow_texture);
        };
    }
}
}
#endif

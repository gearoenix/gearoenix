#ifndef GEAROENIX_RENDER_MODEL_ROOT_STATIC_HPP
#define GEAROENIX_RENDER_MODEL_ROOT_STATIC_HPP
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-uniform.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    namespace mesh {
        class Occ;
    }
    namespace model {
        class RootStatic : public Model {
        private:
            Uniform u;
            std::vector<Model*> children;
            mesh::Occ* occmesh;

        public:
            RootStatic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~RootStatic();
        };
    }
}
}
#endif

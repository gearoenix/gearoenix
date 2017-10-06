#ifndef GEAROENIX_RENDER_MODEL_STATIC_HPP
#define GEAROENIX_RENDER_MODEL_STATIC_HPP
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-uniform.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    namespace mesh {
        class Mesh;
    }
    namespace model {
        class Static : public Model {
        private:
            std::vector<Model*> children;
            mesh::Mesh* mesh;

        public:
            Static(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
        };
    }
}
}
#endif

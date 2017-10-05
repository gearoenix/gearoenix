#ifndef GEAROENIX_RENDER_MODEL_STATIC_HPP
#define GEAROENIX_RENDER_MODEL_STATIC_HPP
#include "rnd-mdl-mesh.hpp"
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-uniform.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    namespace model {
        class Static : public Model {
        private:
            std::vector<Model*> children;
            Mesh* mesh;

        public:
            Static(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
        };
    }
}
}
#endif

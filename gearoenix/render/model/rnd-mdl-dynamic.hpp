#ifndef GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#define GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-occ-mesh.hpp"
#include "rnd-mdl-uniform.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    namespace model {
        class Dynamic : public Model {
        private:
            Uniform u;
            std::vector<Model*> children;
            OccMesh* occmesh;

        public:
            Dynamic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
        };
    }
}
}
#endif

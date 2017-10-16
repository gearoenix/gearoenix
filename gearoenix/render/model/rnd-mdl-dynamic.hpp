#ifndef GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#define GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
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
        class Dynamic : public Model {
        private:
            Uniform u;
            std::vector<Model*> children;
            mesh::Occ* occmesh;

        public:
            Dynamic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            void draw(const std::shared_ptr<camera::Camera>& cam);
            void draw(const std::shared_ptr<camera::Camera>& cam, const Uniform&);
        };
    }
}
}
#endif

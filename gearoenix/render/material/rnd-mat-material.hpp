#ifndef GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../shader/rnd-shd-shader.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace buffer {
        class Uniform;
    }
    namespace pipeline {
        class Pipeline;
    }
    namespace material {
        class Material {
        private:
        protected:
            std::shared_ptr<pipeline::Pipeline> pl;
            buffer::Uniform* ub;
            Material(shader::Id sid, unsigned int us, Engine* e);

        public:
            virtual ~Material();
            virtual unsigned int get_vertex_elements_count() const = 0;
            static Material* read(system::File* f, Engine* e);
        };
    }
}
}
#endif

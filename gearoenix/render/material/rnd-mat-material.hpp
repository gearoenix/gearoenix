#ifndef GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../shader/rnd-shd-shader.hpp"
#include <memory>
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
        class Uniform;
    }
    namespace model {
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
            virtual void update(const model::Uniform& muni) = 0;
            virtual void bind() = 0;
            static Material* read(system::File* f, Engine* e, std::shared_ptr<gearoenix::core::EndCaller> end);
        };
    }
}
}
#endif

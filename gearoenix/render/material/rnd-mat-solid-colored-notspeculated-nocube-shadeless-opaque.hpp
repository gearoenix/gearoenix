#ifndef GEAROEMIX_RENDER_MATERIAL_SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE_HPP
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "../shader/rnd-shd-resources.hpp"
#include "rnd-mat-material.hpp"
#include <memory>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace render {
    namespace texture {
        class Texture2D;
    }
    namespace material {
        class SolidColoredNotspeculatedNocubeShadelessOpaque : public Material {
        public:
            const static core::Id SHADER_ID;
            class Resources : public shader::Resources {
            protected:
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            };
            typedef struct {
                math::Mat4x4 mvp;
                math::Vec3 color;
            } Uniform;

        private:
            Uniform u;
            Resources* shdrsc;

        public:
            SolidColoredNotspeculatedNocubeShadelessOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~SolidColoredNotspeculatedNocubeShadelessOpaque();
            unsigned int get_vertex_elements_count() const;
            void update(const scene::Scene* s, const model::Model* m);
            void bind();
        };
    }
}
}
#endif

#ifndef GEAROENIX_RENDER_MODEL_COPY_HPP
#define GEAROENIX_RENDER_MODEL_COPY_HPP
#include "rnd-mdl-model.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace texture {
        class Texture2D;
    }
    namespace model {
        class RootStatic;
        class Copy : public Model {
        private:
            std::shared_ptr<RootStatic> rs;

        public:
            Copy(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            void commit(const scene::Scene* s);
            void commit(const scene::Scene* s, const Model*);
            void draw(texture::Texture2D* shadow_texture);
        };
    }
}
}
#endif

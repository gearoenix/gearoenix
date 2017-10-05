#ifndef GEAROENIX_RENDER_MODEL_COPY_HPP
#define GEAROENIX_RENDER_MODEL_COPY_HPP
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-uniform.hpp"
#include <memory>
namespace gearoenix {
namespace math {
    class Mat4x4;
}
namespace render {
    namespace model {
        class RootStatic;
        class Copy : public Model {
        private:
            Uniform u;
            std::shared_ptr<RootStatic> rs;

        public:
            Copy(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
        };
    }
}
}
#endif

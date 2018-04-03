#ifndef GEAROEMIX_RENDER_MODEL_STATIC_HPP
#define GEAROEMIX_RENDER_MODEL_STATIC_HPP
#include "../../physics/phs-untransformable.hpp"
#include "rnd-mdl-model.hpp"
namespace gearoenix {
namespace render {
    namespace model {
        class Static : public Model, public physics::Untransferable {
        public:
        protected:
        public:
            Static(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            virtual ~Static();
            // Untransformable---------------------------------------------------------
            void get_location(math::Vec3& l) const;
        };
    }
}
}
#endif

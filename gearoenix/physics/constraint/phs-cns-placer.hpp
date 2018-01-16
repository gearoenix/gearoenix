#ifndef GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP

#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include "phs-cns-constraint.hpp"
#include <memory>
#include <vector>

namespace gearoenix {
namespace render {
    class Model;
}
namespace system {
    class File;
}
namespace physics {
    namespace constraint {
        class Placer : public Constraint {
        private:
            typedef enum : core::Id {
                DOWN_MIDDLE = 33,
                UNKNOWN = 0XFFFFFFFFFFFFFFFF,
            } Type;
            Type t = UNKNOWN;
            core::Real* parameters = nullptr;
            std::vector<std::shared_ptr<render::Model>> models;

        public:
            Placer(system::File* f, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~Placer();
            void on_event(const core::event::Event* e);
        };
    }
}
}
#endif

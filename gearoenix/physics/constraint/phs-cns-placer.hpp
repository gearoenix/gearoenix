#ifndef GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP

#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include "phs-cns-constraint.hpp"
#include <map>
#include <memory>
#include <vector>

namespace gearoenix {
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
            std::map<core::Id, std::shared_ptr<render::model::Model>> models;

        public:
            Placer(system::File* f, render::Engine* render_engine, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~Placer();
            virtual const std::vector<std::pair<core::Id, std::shared_ptr<render::model::Model>>> get_all_models() const;
            virtual void on_event(const core::event::Event* e);
        };
    }
}
}
#endif

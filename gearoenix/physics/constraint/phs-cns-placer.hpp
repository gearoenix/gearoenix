#ifndef GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP

#include "phs-cns-constraint.hpp"
#include "../../core/cr-end-caller.hpp"
#include <vector>
#include <memory>

namespace gearoenix {
	namespace render {
		class Model;
	}
	namespace system {
		class File;
	}
	namespace physics {
		namespace constraint {
			class Placer: public Constraint {
			private:
				core::Id t = 0XFFFFFFFFFFFFFFFF;
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
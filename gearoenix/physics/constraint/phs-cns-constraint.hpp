#ifndef GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
namespace gearoenix {
	namespace core {
		namespace event {
			class Event;
		}
	}
	namespace physics {
		namespace constraint {
			class Constraint {
			private:
			public:
				virtual ~Constraint();
				virtual void on_event(const core::event::Event* e) = 0;
			};
		}
	}
}
#endif
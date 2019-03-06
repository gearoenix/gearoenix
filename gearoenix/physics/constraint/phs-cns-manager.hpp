#ifndef GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix {
namespace render {
	namespace engine {
		class Engine;
	}
}
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace physics {
    namespace constraint {
        class Constraint;
        class Manager {
		protected:
			const std::shared_ptr<render::engine::Engine> e;
			const std::shared_ptr<system::stream::Stream> s;
		public:
			Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<render::engine::Engine> &e);
			~Manager();
			std::shared_ptr<Constraint> get(const core::Id mid, const core::sync::EndCaller<Constraint> c);
        };
    } // namespace constraint
} // namespace physics
} // namespace gearoenix
#endif

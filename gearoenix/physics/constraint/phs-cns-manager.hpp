#ifndef GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix {
namespace render {
    class Engine;
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
            render::Engine* e = nullptr;
            const std::shared_ptr<system::stream::Stream> s;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, render::Engine* e);
            ~Manager();
            std::shared_ptr<Constraint> get(core::Id mid, core::sync::EndCaller<Constraint> c);
        };
    } // namespace constraint
} // namespace physics
} // namespace gearoenix
#endif

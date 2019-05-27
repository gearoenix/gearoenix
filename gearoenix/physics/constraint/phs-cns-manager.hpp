#ifndef GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"

namespace gearoenix {
namespace render::engine {
        class Engine;
    }
namespace system::stream {
        class Stream;
    }
namespace physics::constraint {
        class Constraint;
        class Manager {
        protected:
            render::engine::Engine*const e;
            core::cache::File<Constraint> cache;

        public:
            Manager(system::stream::Stream* s, render::engine::Engine* e) noexcept ;
            ~Manager() noexcept = default;
            std::shared_ptr<Constraint> get_gx3d(core::Id mid, core::sync::EndCaller<Constraint>& c) noexcept ;
        };
    } // namespace physics
} // namespace gearoenix
#endif

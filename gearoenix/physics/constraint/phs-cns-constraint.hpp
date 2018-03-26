#ifndef GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace core {
    namespace event {
        class Event;
    }
}
namespace render {
    class Engine;
    namespace model {
        class Model;
    }
}
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace physics {
    namespace constraint {
        class Placer;
        class Constraint : public core::asset::Asset {
        public:
            typedef enum : core::Id {
                PLACER = 1,
                TRACKER_SPRING_JOINT_SPRING = 2,
                LOOKER = 3,
            } Type;

        protected:
            bool applied = false;
            bool alive = true;
            const Type t;
            Constraint(Type t);

        public:
            virtual ~Constraint();
            virtual void on_event(const core::event::Event& e) = 0;
            virtual const std::vector<std::pair<core::Id, std::shared_ptr<render::model::Model>>> get_all_models() const = 0;
            virtual void apply(core::Real delta_time);
            static Constraint* read(system::stream::Stream* f, render::Engine* render_engine, core::EndCaller<core::EndCallerIgnore> c);
            bool is_alive() const;
            Placer* to_placer();
            const Placer* to_placer() const;
        };
    }
}
}
#endif

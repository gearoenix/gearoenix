#ifndef GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
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
    namespace body {
        class Body;
    }
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
            Constraint(core::Id my_id, Type t);

        public:
            virtual ~Constraint();
            virtual void on_event(const core::event::Event& e) = 0;
            virtual const std::vector<std::pair<core::Id, std::shared_ptr<render::model::Model>>> get_all_models() const = 0;
            virtual const std::vector<std::shared_ptr<body::Body>> get_all_bodies() const;
            virtual void apply(core::Real delta_time);
            static Constraint* read(core::Id my_id, system::stream::Stream* f, render::Engine* render_engine, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            bool is_alive() const;
            Placer* to_placer();
            const Placer* to_placer() const;
        };
    }
}
}
#endif

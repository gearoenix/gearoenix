#ifndef GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "phs-cns-type.hpp"
#include <memory>
#include <vector>

namespace gearoenix {
namespace core::event {
    class Event;
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace model {
        class Dynamic;
    }
}
namespace system::stream {
    class Stream;
}
namespace physics {
    namespace body {
        class Body;
    }
    namespace constraint {
        class Constraint : public core::asset::Asset {
        protected:
            bool applied = false;
            bool alive = true;
            const Type::Id t;

            Constraint(core::Id my_id, Type::Id t);

        public:
            ~Constraint() override;
            virtual void on_event(const core::event::Event& e) = 0;
            [[nodiscard]] virtual std::vector<std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>> get_all_models() const = 0;
            [[nodiscard]] virtual std::vector<std::shared_ptr<body::Body>> get_all_bodies() const;
            virtual void apply(double delta_time) noexcept;

            static Constraint* read(
                core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                render::engine::Engine* e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);

            [[nodiscard]] bool is_alive() const;
        };
    }
}
}
#endif

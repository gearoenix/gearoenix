//#ifndef GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP
//#define GEAROENIX_PHYSICS_CONSTRAINT_PLACER_HPP
//
//#include "../../core/cr-types.hpp"
//#include "../../core/sync/cr-sync-end-caller.hpp"
//#include "../../math/math-vector.hpp"
//#include "phs-cns-constraint.hpp"
//#include <map>
//#include <memory>
//#include <vector>
//
//namespace gearoenix {
//namespace physics {
//    namespace constraint {
//        class Placer : public Constraint {
//        private:
//			// todo change this
//            typedef enum : core::Id {
//                DOWN_MIDDLE = 33,
//                LEFT = 4,
//                RIGHT = 8,
//            } Type;
//            Type t = (Type)0XFFFFFFFFFFFFFFFF;
//            std::vector<core::Real> parameters;
//            math::Vec3 position = math::Vec3(0.0f, 0.0f, 0.0f);
//            math::Vec3 next_position = math::Vec3(0.0f, 0.0f, 0.0f);
//            std::map<core::Id, std::shared_ptr<render::model::Dynamic>> models;
//
//        public:
//            Placer(
//				const core::Id my_id, 
//				const std::shared_ptr<system::stream::Stream> &f,
//				const std::shared_ptr<render::engine::Engine> &e, 
//				const core::sync::EndCaller<core::sync::EndCallerIgnore> c);
//            virtual ~Placer();
//            virtual void apply(core::Real delta_time);
//            virtual const std::vector<std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>> get_all_models() const;
//            virtual void on_event(const core::event::Event& e);
//        };
//    }
//}
//}
//#endif

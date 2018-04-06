#ifndef GEAROEMIX_RENDER_MODEL_DYNAMIC_HPP
#define GEAROEMIX_RENDER_MODEL_DYNAMIC_HPP
#include "../../physics/phs-transformable.hpp"
#include "rnd-mdl-model.hpp"
namespace gearoenix {
namespace render {
    namespace model {
        class Dynamic : public Model, public physics::Transferable {
        private:
            struct State {
                math::Mat4x4 m;
                math::Vec3 x;
                math::Vec3 y;
                math::Vec3 z;
                // TODO it is based on the future needs
            };
            std::vector<State> states;

        public:
            Dynamic(
                core::Id my_id, system::stream::Stream* f, Engine* e,
                core::sync::EndCaller<core::sync::EndCallerIgnore> c,
                const RenderModel::Type t = RenderModel::DYNAMIC);
            virtual ~Dynamic();
            void commit(const scene::Scene* s);
            void push_model_state();
            void pop_model_state();
            // Transformable---------------------------------------------------------
            void get_location(math::Vec3& l) const;
            void set_location(const math::Vec3& l);
            void translate(const math::Vec3& t);
            void global_rotate(const core::Real d, const math::Vec3& axis);
            void global_rotate(const math::Mat4x4& rm);
            void local_rotate(const core::Real d, const math::Vec3& axis);
            void local_x_rotate(const core::Real d);
            void local_y_rotate(const core::Real d);
            void local_z_rotate(const core::Real d);
            void global_scale(const core::Real s);
            void local_scale(const core::Real s);
        };
    }
}
}
#endif

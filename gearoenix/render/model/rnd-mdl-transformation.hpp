#ifndef GEAROENIX_RENDER_MODEL_TRANSFORMATON_HPP
#define GEAROENIX_RENDER_MODEL_TRANSFORMATON_HPP
#include "../../physics/phs-transformation.hpp"

namespace gearoenix {
namespace render {
    namespace model {
        struct Uniform;
        class Transformation : public physics::Transformation {
        private:
            Uniform* const uniform;

        public:
            Transformation(Uniform* const uniform) noexcept;
            ~Transformation() noexcept override final;
            // physics::Transformation----------------------------------------------------------------------------
            //const math::Vec3& get_x_axis() const noexcept override final;
            //const math::Vec3& get_y_axis() const noexcept override final;
            //const math::Vec3& get_z_axis() const noexcept override final;
            //const math::Vec3& get_location() const noexcept override final;
            //void get_location(math::Vec3& l) const noexcept override final;
            void set_location(const math::Vec3& l) noexcept override final;
            //void translate(const math::Vec3& t) noexcept override final;
            //void local_x_translate(const core::Real t) noexcept override final;
            //void local_y_translate(const core::Real t) noexcept override final;
            //void local_z_translate(const core::Real t) noexcept override final;
            //void global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept override final;
            //void global_rotate(const core::Real d, const math::Vec3& axis) noexcept override final;
            //void local_rotate(const core::Real d, const math::Vec3& axis) noexcept override final;
            //void local_x_rotate(const core::Real d) noexcept override final;
            //void local_y_rotate(const core::Real d) noexcept override final;
            //void local_z_rotate(const core::Real d) noexcept override final;
            //void set_orientation(const math::Quat& q) noexcept override final;
        };
    }
}
}
#endif

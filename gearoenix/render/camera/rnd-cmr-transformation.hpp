#ifndef GEAROENIX_RENDER_CAMERA_TRANSFORMATON_HPP
#define GEAROENIX_RENDER_CAMERA_TRANSFORMATON_HPP
#include "../../physics/phs-transformation.hpp"

namespace gearoenix {
namespace render {
    namespace camera {
        struct Uniform;
        class Transformation : public physics::Transformation {
        private:
            const std::shared_ptr<Uniform> uniform;

        public:
            Transformation(const std::shared_ptr<Uniform>& uniform) noexcept;
            ~Transformation() noexcept override final;
            void update_location() noexcept;
            void update_view_projection() noexcept;
            void look_at(const math::Vec3& target, const math::Vec3& up) noexcept;
            void look_at(const math::Vec3& origin, const math::Vec3& target, const math::Vec3& up) noexcept;
            // physics::Transformation----------------------------------------------------------------------------
            const math::Vec3& get_x_axis() const noexcept override final;
            const math::Vec3& get_y_axis() const noexcept override final;
            const math::Vec3& get_z_axis() const noexcept override final;
            const math::Vec3& get_location() const noexcept override final;
            void get_location(math::Vec3& l) const noexcept override final;
            void set_location(const math::Vec3& l) noexcept override final;
            void translate(const math::Vec3& t) noexcept override final;
            void local_x_translate(const core::Real t) noexcept override final;
            void local_y_translate(const core::Real t) noexcept override final;
            void local_z_translate(const core::Real t) noexcept override final;
            void global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept override final;
            void global_rotate(const core::Real d, const math::Vec3& axis) noexcept override final;
            void local_rotate(const core::Real d, const math::Vec3& axis) noexcept override final;
            void local_x_rotate(const core::Real d) noexcept override final;
            void local_y_rotate(const core::Real d) noexcept override final;
            void local_z_rotate(const core::Real d) noexcept override final;
            void set_orientation(const math::Quat& q) noexcept override final;
        };
    }
}
}
#endif

#ifndef GEAROENIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#define GEAROENIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
        class Orthographic : public Camera {
        private:
            core::Real aspects_size;
            void on_ratio_change();

        protected:
        public:
            Orthographic(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e);
            bool in_sight(const math::Vec3& location, const core::Real radius) const;
            math::Ray3 create_ray3(const core::Real x, const core::Real y) const;
            core::Real get_distance(const math::Vec3& model_location) const final override;
        };
    }
}
}
#endif

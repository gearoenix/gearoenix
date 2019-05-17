#ifndef GEAROENIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#define GEAROENIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix::render::camera {
class Orthographic : public Camera {
private:
    core::Real aspects_size;
    void on_ratio_change();

protected:
public:
    Orthographic(
        core::Id my_id,
        const std::shared_ptr<system::stream::Stream>& f,
        const std::shared_ptr<engine::Engine>& e);
    math::Ray3 create_ray3(core::Real x, core::Real y) const final;
    core::Real get_distance(const math::Vec3& model_location) const final;
};
}
#endif

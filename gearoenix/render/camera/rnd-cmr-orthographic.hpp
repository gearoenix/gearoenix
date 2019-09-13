#ifndef GEAROENIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#define GEAROENIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix::render::camera {
class Orthographic : public Camera {
private:
    core::Real aspects_size = 1.0f;
    void update_aspects_size() noexcept;
    void update_cascades() noexcept;

protected:
public:
    Orthographic(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;
    Orthographic(core::Id my_id, engine::Engine* e) noexcept;
    math::Ray3 create_ray3(core::Real x, core::Real y) const noexcept final;
    core::Real get_distance(const math::Vec3& model_location) const noexcept final;
    void set_aspects_size(core::Real aspects_size) noexcept;
    void set_aspect_ratio(core::Real aspect_ratio) noexcept final;
};
}
#endif

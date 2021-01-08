#ifndef GEAROENIX_RENDER_CAMERA_PERSPECTIVE_HPP
#define GEAROENIX_RENDER_CAMERA_PERSPECTIVE_HPP
#include "gx-rnd-cmr-camera.hpp"

namespace gearoenix::physics::collider {
struct Frustum;
}

namespace gearoenix::render::camera {
struct Perspective : public Camera {
private:
    double fovx = 1.0;
    double fovy = 1.0;
    double tanx = 1.0;
    double tany = 1.0;
    double lambda = 0.3;

    void update_fovy() noexcept;
    void update_cascades() noexcept;
    void update_projection() noexcept;

public:
    Perspective(core::Id my_id, std::string name, platform::stream::Stream* f, engine::Engine* e) noexcept;
    Perspective(core::Id my_id, std::string name, engine::Engine* e) noexcept;
    void set_aspects(unsigned int w, unsigned int h) noexcept final;
    void set_field_of_view(double radian) noexcept;
    [[nodiscard]] math::Ray3 create_ray3(double x, double y) const noexcept final;
    [[nodiscard]] double get_distance(const math::Vec3<double>& model_location) const noexcept final;
};
}
#endif

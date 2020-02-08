#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-listener.hpp"
#include "../../math/math-ray.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "rnd-cmr-uniform.hpp"
#include <array>
#include <memory>
#include <vector>

namespace gearoenix::core::event {
class Event;
}

namespace gearoenix::math {
struct ProjectorFrustum;
struct Quat;
}

namespace gearoenix::physics {
class Transformation;
}

namespace gearoenix::physics::collider {
class Collider;
class Frustum;
}

namespace gearoenix::system {
class Application;
class File;
}

namespace gearoenix::render::buffer {
class FramedUniform;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::texture {
class Target;
}

namespace gearoenix::render::camera {
class Camera : public core::asset::Asset, public core::event::Listener {
public:
    typedef std::array<math::Vec3, 4> Partition;

    GX_GETSET_VAL_PRT(core::Real, layer, 0.0f)
    GX_GET_UPTR_PRT(physics::collider::Frustum, frustum_collider)
    GX_GET_UPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GET_CREF_PRT(std::vector<Partition>, cascaded_shadow_frustum_partitions)
    GX_GET_CREF_PRT(Uniform, uniform)
    GX_GET_UPTR_PRT(physics::Transformation, transformation)
    GX_GETSET_CREF_PRT(std::shared_ptr<texture::Target>, target)
    GX_GETSET_VAL_PRT(bool, enabled, true)
protected:
    engine::Engine* const e;

    void initialize() noexcept;

    Camera(core::Id my_id, engine::Engine* e) noexcept;
    Camera(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;

public:
    ~Camera() noexcept override;
    void set_far(core::Real f) noexcept;
    virtual void update_uniform() noexcept;
    virtual void set_aspect_ratio(core::Real ratio) noexcept;
    [[nodiscard]] virtual math::Ray3 create_ray3(core::Real x, core::Real y) const noexcept = 0;
    [[nodiscard]] virtual core::Real get_distance(const math::Vec3& model_location) const noexcept = 0;

    bool on_event(const core::event::Data& d) noexcept override;
};
}
#endif

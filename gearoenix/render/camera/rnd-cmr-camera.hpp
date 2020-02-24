#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-pool.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-listener.hpp"
#include "../../math/math-ray.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../material/rnd-mat-type.hpp"
#include "rnd-cmr-uniform.hpp"
#include <array>
#include <memory>
#include <tuple>
#include <vector>

namespace gearoenix::core::event {
class Event;
}

namespace gearoenix::math {
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

namespace gearoenix::physics::accelerator {
class Bvh;
}

namespace gearoenix::render::buffer {
class FramedUniform;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::light {
class CascadeInfo;
class Directional;
}

namespace gearoenix::render::mesh {
class Mesh;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::texture {
class Target;
}

namespace gearoenix::render::camera {
class Camera : public core::asset::Asset, public core::event::Listener {
public:
    typedef std::array<math::Vec3, 4> Partition;
    typedef std::vector<std::tuple<const material::Type, model::Model*const, mesh::Mesh*const>> Meshes;
    typedef std::vector<std::tuple<const core::Real, const material::Type, model::Model*const, mesh::Mesh*const>> TransparentMeshes;

    GX_GETSET_VAL_PRT(core::Real, layer, 0.0f)
    GX_GET_UPTR_PRT(physics::collider::Frustum, frustum_collider)
    GX_GET_UPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GET_CREF_PRT(std::vector<Partition>, cascaded_shadow_frustum_partitions)
    GX_GET_CREF_PRT(Uniform, uniform)
    GX_GET_UPTR_PRT(physics::Transformation, transformation)
    GX_GET_UPTR_PRT(texture::Target, target)
    GX_GETSET_VAL_PRT(bool, enabled, true)
    GX_GETSET_VAL_PRT(bool, cascaded_shadow_enabled, false)
    GX_GET_CREF_PRT(Meshes, seen_static_opaque_meshes) // sorted
    GX_GET_CREF_PRT(TransparentMeshes, seen_static_transparent_meshes) // sorted
    GX_GET_CREF_PRT(Meshes, seen_dynamic_opaque_meshes) // sorted
    GX_GET_CREF_PRT(TransparentMeshes, seen_dynamic_transparent_meshes) // sorted
    GX_GET_CREF_PRT(Meshes, seen_opaque_meshes) // sorted
    GX_GET_CREF_PRT(TransparentMeshes, seen_transparent_meshes) // sorted
    GX_GET_REF_PRT(core::OneLoopPool<light::CascadeInfo>, cascades)
protected:
    engine::Engine* const e;

    void initialize() noexcept;

    Camera(core::Id my_id, engine::Engine* e) noexcept;
    Camera(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;

    void config_target() const noexcept;

public:
    ~Camera() noexcept override;
    void set_far(core::Real f) noexcept;
    void set_target(const texture::Target* target) noexcept;
    virtual void update() noexcept;
    virtual void set_aspect_ratio(core::Real ratio) noexcept;
    [[nodiscard]] virtual math::Ray3 create_ray3(core::Real x, core::Real y) const noexcept = 0;
    [[nodiscard]] virtual core::Real get_distance(const math::Vec3& model_location) const noexcept = 0;
    void check_static_models(const physics::accelerator::Bvh* bvh) noexcept;
    void check_dynamic_models(const physics::accelerator::Bvh* bvh) noexcept;
    void cascade_shadow(const light::Directional* l) noexcept;
    void merge_seen_meshes() noexcept;

    bool on_event(const core::event::Data& d) noexcept override;
};
}
#endif

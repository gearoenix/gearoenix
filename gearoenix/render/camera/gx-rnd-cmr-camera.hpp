#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/gx-cr-asset.hpp"
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "../../core/gx-cr-pool.hpp"
#include "../../core/gx-cr-static.hpp"
#include "../../math/gx-math-ray.hpp"
#include "../material/gx-rnd-mat-type.hpp"
#include "gx-rnd-cmr-uniform.hpp"
#include <array>
#include <memory>
#include <tuple>
#include <vector>

namespace gearoenix::core::event {
class Event;
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
struct Bvh;
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

namespace gearoenix::render::model {
class Model;
class Mesh;
}

namespace gearoenix::render::texture {
class Target;
}

namespace gearoenix::render::camera {
class Transformation;
class Camera : public core::asset::Asset, public core::event::Listener {
public:
    typedef std::array<math::Vec3<double>, 4> Partition;
    typedef std::vector<std::tuple<double, material::Type, model::Model*, model::Mesh*>> Meshes;

    GX_GETSET_VAL_PRT(double, layer, 0.0)
    GX_GET_UPTR_PRT(physics::collider::Frustum, frustum_collider)
    GX_GET_UPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GET_VAL_PRT(std::optional<std::size_t>, cascaded_shadow_frustum_partitions_count, std::nullopt)
    GX_GET_CREF_PRT(std::vector<Partition>, cascaded_shadow_frustum_partitions)
    GX_GET_CREF_PRT(Uniform, uniform)
    GX_GET_UPTR_PRT(Transformation, transformation)
    GX_GET_UPTR_PRT(texture::Target, target)
    GX_GETSET_VAL_PRT(bool, enabled, true)
    GX_GETSET_VAL_PRT(bool, cascaded_shadow_enabled, false)
    GX_GET_CREF_PRT(Meshes, seen_static_opaque_meshes) // sorted
    GX_GET_CREF_PRT(Meshes, seen_static_transparent_meshes) // sorted
    GX_GET_CREF_PRT(Meshes, seen_dynamic_opaque_meshes) // sorted
    GX_GET_CREF_PRT(Meshes, seen_dynamic_transparent_meshes) // sorted
    GX_GET_CREF_PRT(Meshes, seen_transparent_meshes) // sorted
    GX_GET_REF_PRT(core::OneLoopPool<light::CascadeInfo>, cascades)
    GX_GET_CPTR_PRT(engine::Engine, render_engine)
protected:
    void initialize() noexcept;

    Camera(core::Id my_id, std::string name, engine::Engine* e) noexcept;
    Camera(core::Id my_id, std::string name, system::stream::Stream* f, engine::Engine* e) noexcept;

    void config_target() const noexcept;

public:
    ~Camera() noexcept override;
    void set_far(float f) noexcept;
    [[nodiscard]] float get_far() const noexcept;
    void set_near(float f) noexcept;
    [[nodiscard]] float get_near() const noexcept;
    void set_gamma_correction_factor(float f) noexcept;
    void set_target(const texture::Target* target) noexcept;
    void set_cascaded_shadow_frustum_partitions_count(std::size_t c) noexcept;
    virtual void update() noexcept;
    virtual void set_aspects(unsigned int w, unsigned int h) noexcept;
    [[nodiscard]] virtual math::Ray3 create_ray3(double x, double y) const noexcept = 0;
    [[nodiscard]] virtual double get_distance(const math::Vec3<double>& model_location) const noexcept = 0;
    void check_static_models(const physics::accelerator::Bvh* bvh) noexcept;
    void check_dynamic_models(const physics::accelerator::Bvh* bvh) noexcept;
    void cascade_shadow(const light::Directional* l) noexcept;
    void merge_seen_meshes() noexcept;

    bool on_event(const core::event::Data& d) noexcept override;
};
}
#endif

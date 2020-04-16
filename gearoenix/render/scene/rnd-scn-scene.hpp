#ifndef GEAROENIX_RENDER_SCENE_SCENE_HPP
#define GEAROENIX_RENDER_SCENE_SCENE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-scn-type.hpp"
#include "rnd-scn-uniform.hpp"
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

namespace gearoenix::audio {
class Audio;
}

namespace gearoenix::core::event {
class Event;
}

namespace gearoenix::math {
struct Ray3;
}

namespace gearoenix::physics::accelerator {
class Bvh;
}
namespace gearoenix::physics::body {
class Body;
class Rigid;
}
namespace gearoenix::physics::collider {
class Collider;
}
namespace gearoenix::physics::constraint {
class Constraint;
}

namespace gearoenix::system {
class File;
}

namespace gearoenix::render::buffer {
class FramedUniform;
}

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::light {
class Light;
class Directional;
}

namespace gearoenix::render::material {
class Material;
}

namespace gearoenix::render::mesh {
class Mesh;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::reflection {
class Baked;
class Reflection;
class Runtime;
}

namespace gearoenix::render::skybox {
class Skybox;
}

namespace gearoenix::render::texture {
class Texture2D;
}

namespace gearoenix::render::scene {
class Scene : public core::asset::Asset {
public:
    using MapAudio = std::map<core::Id, std::shared_ptr<audio::Audio>>;
    using MapCamera = std::map<core::Id, std::shared_ptr<camera::Camera>>;
    using MapConstraint = std::map<core::Id, std::shared_ptr<physics::constraint::Constraint>>;
    using MapLight = std::map<core::Id, std::shared_ptr<light::Light>>;
    using MapShadowCascaderLight = std::map<core::Id, std::shared_ptr<light::Directional>>;
    using MapModel = std::map<core::Id, std::shared_ptr<model::Model>>;
    using MapSkybox = std::map<core::Id, std::shared_ptr<skybox::Skybox>>;
    using MapReflection = std::map<core::Id, std::shared_ptr<reflection::Reflection>>;
    using MapRuntimeReflection = std::map<core::Id, std::shared_ptr<reflection::Runtime>>;

    GX_GET_CVAL_PRT(Type, scene_type_id)
    GX_GETSET_VAL_PRT(double, layer, 0.0)
    GX_GETSET_VAL_PRT(bool, enability, false)
    GX_GETSET_VAL_PRT(bool, models_changed, false)
    GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GET_UCPTR_PRT(physics::accelerator::Bvh, static_accelerator)
    GX_GET_UCPTR_PRT(physics::accelerator::Bvh, dynamic_accelerator)
    GX_GET_CREF_PRT(MapAudio, audios)
    GX_GET_CREF_PRT(MapCamera, cameras)
    GX_GET_CREF_PRT(MapConstraint, constraints)
    GX_GET_CREF_PRT(MapLight, lights)
    GX_GET_CREF_PRT(MapShadowCascaderLight, shadow_cascader_lights)
    GX_GET_CREF_PRT(MapModel, models)
    GX_GET_CREF_PRT(MapSkybox, skyboxs)
    GX_GET_CREF_PRT(MapReflection, reflections)
    GX_GET_CREF_PRT(MapRuntimeReflection, runtime_reflections)
    GX_GET_CREF_PRT(std::shared_ptr<reflection::Baked>, default_reflection_probe)
    GX_GET_CREF_PRT(std::vector<physics::collider::Collider*>, static_colliders)
    GX_GET_CREF_PRT(std::vector<physics::collider::Collider*>, dynamic_colliders)
protected:
    engine::Engine* const e;
    Uniform uniform;

    Scene(
        core::Id my_id, Type t, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Scene(
        core::Id my_id, Type t, system::stream::Stream* f, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    ~Scene() noexcept override;

#define GX_HELPER(x, c)                                         \
protected:                                                      \
    void scene_add_##x(const std::shared_ptr<c>& m) noexcept;   \
                                                                \
public:                                                         \
    virtual void add_##x(const std::shared_ptr<c>& m) noexcept; \
    [[nodiscard]] const std::shared_ptr<c>& get_##x(core::Id x##_id) const noexcept;

    GX_HELPER(camera, camera::Camera)
    GX_HELPER(audio, audio::Audio)
    GX_HELPER(light, light::Light)
    GX_HELPER(model, model::Model)
    GX_HELPER(constraint, physics::constraint::Constraint)
    GX_HELPER(skybox, skybox::Skybox)
    GX_HELPER(reflection, reflection::Reflection)

#undef GX_HELPER

    // void set_skybox(std::shared_ptr<skybox::Skybox> s) noexcept;

    void update() noexcept;
    [[nodiscard]] std::optional<std::pair<double, physics::collider::Collider*>> hit(const math::Ray3& r, double d_min) const noexcept;
    void add_shadow_cascader(core::Id light_id) noexcept;
    void remove_shadow_cascader(core::Id light_id) noexcept;
    void set_default_reflection_probe(std::shared_ptr<reflection::Baked> rfl) noexcept;
};
}
#endif

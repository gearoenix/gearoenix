#ifndef GEAROENIX_RENDER_MODEL_MODEL_HPP
#define GEAROENIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-mdl-type.hpp"
#include <map>
#include <memory>
#include <vector>

namespace gearoenix::render::buffer {
class FramedUniform;
}

namespace gearoenix::physics::collider {
class Collider;
}

namespace gearoenix::physics {
class Transformation;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::render::texture {
class TextureCube;
}

namespace gearoenix::render::reflection {
class Reflection;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::model {
class Mesh;
/// Static models will get static reflection when it was being added to scene
/// Dynamic models will get reflection in each frame if the locked reflection is null
///   and exactly residing inside a reflection collider
class Model : public core::asset::Asset {
public:
    using MapMesh = std::map<core::Id, std::shared_ptr<Mesh>>;
    using MapModel = std::map<core::Id, std::shared_ptr<Model>>;

    GX_GET_CVAL_PRT(Type, model_type)
    GX_GET_UPTR_PRT(physics::collider::Collider, collider)
    GX_GET_UPTR_PRT(physics::Transformation, transformation)
    GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GET_VAL_PRT(bool, has_shadow_caster, false)
    GX_GET_VAL_PRT(bool, has_transparent, false)
    GX_GET_VAL_PRT(bool, enabled, true)
    GX_GET_CREF_PRT(MapMesh, meshes)
    GX_GET_CREF_PRT(MapModel, children)
    GX_GET_PTR_PRT(Model, parent)
    GX_GET_PTR_PRT(scene::Scene, scene)
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_VAL_PRT(unsigned int, latest_frame_update, static_cast<unsigned int>(-1))
    /// Thw AABB of reflection must surrounds the model
    /// and it is one of the children of the model
    GX_GET_CREF_PRT(std::shared_ptr<reflection::Reflection>, hooked_reflection)
    /// In case the upper was not set, the following must be found and used
    GX_GET_PTR_PRT(reflection::Reflection, colliding_reflection)
protected:
    Model(
        core::Id my_id,
        Type t,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Model(
        core::Id my_id,
        Type t,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

    void set_reflection(texture::TextureCube* irradiance, texture::TextureCube* radiance) noexcept;

public:
    ~Model() noexcept override;
    virtual void update() noexcept;
    void add_mesh(const std::shared_ptr<Mesh>& m) noexcept;
    void add_child(const std::shared_ptr<Model>& c) noexcept;
    void set_collider(std::unique_ptr<physics::collider::Collider> c) noexcept;
    void set_enabled(bool s) noexcept;
    void set_hooked_reflection(std::shared_ptr<reflection::Reflection> reflection_probe) noexcept;
    void set_colliding_reflection(reflection::Reflection* reflection_probe) noexcept;
    void clear_reflection() noexcept;
    virtual void set_scene(scene::Scene* s) noexcept;
    [[nodiscard]] virtual bool get_dynamicity() const noexcept = 0;
    Model* get_root() noexcept;
    const Model* get_root() const noexcept;
};
}
#endif

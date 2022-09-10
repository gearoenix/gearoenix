#ifndef GEAROENIX_RENDER_SCENE_SCENE_HPP
#define GEAROENIX_RENDER_SCENE_SCENE_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::light {
struct Light;
}

namespace gearoenix::render::model {
struct Model;
}

namespace gearoenix::render::reflection {
struct Baked;
struct Runtime;
}

namespace gearoenix::render::skybox {
struct Skybox;
}

namespace gearoenix::render::scene {
struct Scene final : public core::ecs::Component {
    GX_GET_RRF_PRV(engine::Engine, e)
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::Entity::id_t>, entities)
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::Entity::id_t>, model_entities)
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::Entity::id_t>, camera_entities)
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::Entity::id_t>, baked_reflection_entities)
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::Entity::id_t>, runtime_reflection_entities)
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::Entity::id_t>, skybox_entities)
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::Entity::id_t>, light_entities)
    // radius, normal-jitter, min and max depth values for occlusion
    GX_GET_REF_PRV(math::Vec4<float>, ssao_settings)
    GX_GETSET_VAL_PRV(double, layer, 0.0)
    GX_GET_VAL_PRV(bool, recreate_bvh, true)
    GX_GET_VAL_PRV(bool, reflection_probs_changed, true) // TODO: later must be a mechanism to update it
    boost::container::flat_map<std::string, math::Vec3<double>> empties;

private:
    boost::container::flat_map<core::ecs::Entity::id_t, std::uint64_t> cameras_flags;

public:
    Scene(engine::Engine& e, double layer) noexcept;
    ~Scene() noexcept final;
    Scene(Scene&&) noexcept;
    void add_model(core::ecs::Entity::id_t entity, model::Model& m) noexcept;
    void add_camera(core::ecs::Entity::id_t entity, camera::Camera& c) noexcept;
    void add_baked_reflection(core::ecs::Entity::id_t entity, reflection::Baked& b) noexcept;
    void add_runtime_reflection(core::ecs::Entity::id_t entity, reflection::Runtime& r) noexcept;
    void add_skybox(core::ecs::Entity::id_t entity, skybox::Skybox& s) noexcept;
    void add_light(core::ecs::Entity::id_t entity, light::Light& l) noexcept;
    void update(core::ecs::Entity::id_t scene_entity_id) noexcept;
};
}
#endif

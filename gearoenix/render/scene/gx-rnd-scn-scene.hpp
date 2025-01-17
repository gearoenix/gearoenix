#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
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
struct Scene final : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 24;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_RRF_PRV(engine::Engine, e);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, armature_entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, baked_reflection_entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, camera_entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, empty_entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, light_entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, model_entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, runtime_reflection_entities);
    GX_GET_CREF_PRV(boost::container::flat_set<core::ecs::entity_id_t>, skybox_entities);
    // radius, normal-jitter, min and max depth values for occlusion
    GX_GET_REF_PRV(math::Vec4<float>, ssao_settings);
    GX_GETSET_VAL_PRV(double, layer, 0.0);
    GX_GET_VAL_PRV(bool, recreate_bvh, true);
    GX_GET_VAL_PRV(bool, reflection_probs_changed, true); // TODO: later must find a mechanism to update it

    boost::container::flat_map<core::ecs::entity_id_t, std::uint64_t> cameras_flags;

public:
    Scene(engine::Engine& e, double layer, std::string&& name, core::ecs::entity_id_t entity_id);
    ~Scene() override;
    void add_model(core::ecs::entity_id_t entity, model::Model& m);
    void add_camera(core::ecs::entity_id_t entity, camera::Camera& c);
    void add_baked_reflection(core::ecs::entity_id_t entity, reflection::Baked& b);
    void add_runtime_reflection(core::ecs::entity_id_t entity, reflection::Runtime& r);
    void add_skybox(core::ecs::entity_id_t entity, skybox::Skybox& s);
    void add_light(core::ecs::entity_id_t entity, light::Light& l);
    void add_empty(core::ecs::entity_id_t entity);
    void add_armature(core::ecs::entity_id_t entity);
    void add_entity(core::ecs::entity_id_t entity);
    void update(core::ecs::entity_id_t scene_entity_id);
};
}

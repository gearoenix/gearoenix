#ifndef GEAROENIX_GL_SUBMISSION_SCENE_HPP
#define GEAROENIX_GL_SUBMISSION_SCENE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "gx-gl-sbm-animation.hpp"
#include "gx-gl-sbm-environment.hpp"
#include "gx-gl-sbm-light.hpp"
#include "gx-gl-sbm-model.hpp"
#include <boost/container/flat_map.hpp>

namespace gearoenix::gl::submission {
struct Scene final {
    math::Vec4<float> ssao_settings;
    boost::container::flat_map<std::tuple<double /*layer*/, core::ecs::entity_id_t /*skybox-entity-id*/, bool /*equirectangular*/>, Skybox> skyboxes;
    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::entity_id_t /*camera-entity-id*/>, std::size_t /*camera-pool-index*/> cameras;
    boost::container::flat_map<core::ecs::entity_id_t /*camera-id*/, std::size_t /*camera-pool-index*/> reflection_cameras;
    boost::container::flat_map<core::ecs::entity_id_t /*camera-id*/, std::size_t /*camera-pool-index*/> shadow_cameras;
    boost::container::flat_map<core::ecs::entity_id_t /*reflection-id*/, Reflection> reflections;
    boost::container::flat_map<core::ecs::entity_id_t /*light-id*/, DirectionalShadowCaster> shadow_caster_directional_lights;
    std::pair<core::ecs::entity_id_t /*reflection-id*/, Reflection> default_reflection { 0, Reflection {} };
    std::vector<DynamicModel> dynamic_models;
    std::vector<DebugModel> debug_mesh_data;
    std::vector<Bone> bones_data;
    const std::string* name = nullptr;
};
}

#endif
#endif
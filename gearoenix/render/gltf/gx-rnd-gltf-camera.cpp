#include "gx-rnd-gltf-camera.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"

gearoenix::render::gltf::Cameras::Cameras(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Cameras::~Cameras() = default;

bool gearoenix::render::gltf::Cameras::is_camera(const int node_index) const
{
    if (const auto& node = context.data.nodes[node_index]; node.camera != -1) {
        GX_ASSERT_D(node.children.empty());
        GX_ASSERT_D(node.emitter == -1);
        GX_ASSERT_D(node.light == -1);
        GX_ASSERT_D(node.mesh == -1);
        GX_ASSERT_D(node.skin == -1);
        return true;
    }
    return false;
}

bool gearoenix::render::gltf::Cameras::process(const int node_index, core::ecs::Entity* const parent, const core::job::EndCaller<>& end_callback) const
{
    const auto& node = context.data.nodes[node_index];
    if (!is_camera(node_index)) {
        return false;
    }
    const auto& cmr = context.data.cameras[node.camera];
    GX_LOG_D("Loading camera: " << cmr.name);
    camera::Manager::get().build(std::string(cmr.name), parent, core::job::EndCaller<core::ecs::EntityPtr>([this, node_index, end_callback](auto&& camera_entity) {
        const auto& node = context.data.nodes[node_index];
        const auto& cmr = context.data.cameras[node.camera];
        auto* const rnd_cmr = camera_entity->template get_component<camera::Camera>();
        if ("perspective" == cmr.type) {
            GX_ASSERT_D(cmr.perspective.znear < cmr.perspective.zfar);
            GX_ASSERT_D(cmr.perspective.znear > 0.0);
            rnd_cmr->set_projection_data(camera::ProjectionData::construct_perspective(static_cast<float>(cmr.perspective.yfov)));
            rnd_cmr->set_far(static_cast<float>(cmr.perspective.zfar));
            rnd_cmr->set_near(static_cast<float>(cmr.perspective.znear));
        } else {
            GX_ASSERT_D(cmr.orthographic.xmag == cmr.orthographic.ymag);
            GX_ASSERT_D(cmr.orthographic.xmag > 0.0);
            rnd_cmr->set_projection_data(camera::ProjectionData::construct_orthographic(static_cast<float>(cmr.orthographic.xmag)));
            rnd_cmr->set_far(static_cast<float>(cmr.orthographic.zfar));
            rnd_cmr->set_near(static_cast<float>(cmr.orthographic.znear));
        }
        apply_transform(node_index, context, *camera_entity->template get_component<physics::Transformation>());
        (void)end_callback;
    }));
    return true;
}

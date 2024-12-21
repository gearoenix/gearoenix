#include "gx-rnd-gltf-camera.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
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

bool gearoenix::render::gltf::Cameras::process(
    const int node_index,
    physics::Transformation* const parent_transform,
    const core::job::EndCaller<>& gpu_end_callback,
    const core::job::EndCaller<>& entity_end_callback,
    const std::shared_ptr<scene::Builder>& scene_builder) const
{
    const auto& node = context.data.nodes[node_index];
    if (!is_camera(node_index)) {
        return false;
    }
    const auto& cmr = context.data.cameras[node.camera];
    GX_LOG_D("Loading camera: " << cmr.name);
    engine::Engine::get()->get_camera_manager()->build(
        cmr.name,
        parent_transform,
        core::job::EndCallerShared<camera::Builder>([this, node_index, scene_builder, gpu_end_callback](std::shared_ptr<camera::Builder>&& camera_builder) {
            const auto& node = context.data.nodes[node_index];
            const auto& cmr = context.data.cameras[node.camera];
            auto& rnd_cmr = camera_builder->get_camera();
            if ("perspective" == cmr.type) {
                GX_ASSERT_D(cmr.perspective.znear < cmr.perspective.zfar);
                GX_ASSERT_D(cmr.perspective.znear > 0.0);
                rnd_cmr.set_projection_data(camera::ProjectionData::construct_perspective(static_cast<float>(cmr.perspective.yfov)));
                rnd_cmr.set_far(static_cast<float>(cmr.perspective.zfar));
                rnd_cmr.set_near(static_cast<float>(cmr.perspective.znear));
            } else {
                GX_ASSERT_D(cmr.orthographic.xmag == cmr.orthographic.ymag);
                GX_ASSERT_D(cmr.orthographic.xmag > 0.0);
                rnd_cmr.set_projection_data(camera::ProjectionData::construct_orthographic(static_cast<float>(cmr.orthographic.xmag)));
                rnd_cmr.set_far(static_cast<float>(cmr.orthographic.zfar));
                rnd_cmr.set_near(static_cast<float>(cmr.orthographic.znear));
            }
            auto& transform = camera_builder->get_transformation();
            apply_transform(node_index, context, transform);
            scene_builder->add(std::move(camera_builder));
            (void)gpu_end_callback;
        }),
        core::job::EndCaller(entity_end_callback));
    return true;
}

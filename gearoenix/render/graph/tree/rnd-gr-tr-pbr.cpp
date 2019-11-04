#include "rnd-gr-tr-pbr.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../physics/phs-engine.hpp"
#include "../../../system/sys-app.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-cascade-info.hpp"
#include "../../light/rnd-lt-directional.hpp"
#include "../../scene/rnd-scn-manager.hpp"
#include "../../scene/rnd-scn-scene.hpp"
#include "../node/rnd-gr-nd-forward-pbr.hpp"
#include "../node/rnd-gr-nd-shadow-mapper.hpp"
#include <memory>

gearoenix::render::graph::tree::Pbr::Pbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
    : Tree(e)
    , in_weak_hardware(engine::Type::OPENGL_ES2 == e->get_engine_type())
{
}

gearoenix::render::graph::tree::Pbr::~Pbr() noexcept
{
    GXLOGD("Pbr render tree deleted.")
}

void gearoenix::render::graph::tree::Pbr::update() noexcept
{
    cascades.clear();
    fwd.refresh();
    nodes.clear();
    const auto& priorities_scenes = e->get_physics_engine()->get_visible_models().priority_ptr_scene;
    for (const auto& priority_scenes : priorities_scenes) {
        const core::Real scene_priority = priority_scenes.first;
        auto& scene_priority_nodes = nodes[scene_priority];
        const auto& scenes_priorities_cameras = priority_scenes.second;
        for (const auto& scene_priorities_cameras : scenes_priorities_cameras) {
            const scene::Scene* const scn = scene_priorities_cameras.first;
            auto& scene_nodes = scene_priority_nodes[scn];
            const auto& priorities_cameras = scene_priorities_cameras.second.priority_ptr_camera;
            for (const auto& priority_cameras : priorities_cameras) {
                const core::Real camera_priority = priority_cameras.first;
                const auto& cameras_data = priority_cameras.second;
                auto& camera_priority_nodes = scene_nodes[camera_priority];
                for (const auto& camera_data : cameras_data) {
                    const camera::Camera* const cam = camera_data.first;
                    node::ForwardPbr* const n = fwd.get_next([this] {
                        auto* const n = new node::ForwardPbr(
                            e,
                            core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                        n->set_render_target(e->get_main_render_target());
                        return n;
                        });
                    camera_priority_nodes[cam] = n;
                    n->set_scene(scn);
                    n->set_camera(cam);
                    const auto& models_lights = camera_data.second;
                    const auto& shadow_caster_directional_lights = models_lights.shadow_caster_directional_lights;
                    n->set_directional_lights(&shadow_caster_directional_lights);
                    n->set_opaque_models(&models_lights.opaque_container_models);
                    n->set_transparent_models(&models_lights.transparent_container_models);
                    n->update();
                    for (const auto& priority_lights : shadow_caster_directional_lights) {
                        const auto& lights = priority_lights.second;
                        for (const auto& light_cascasdes : lights) {
                            cascades.push_back(light_cascasdes.second);
                        }
                    }
                }
            }
        }
    }
}

void gearoenix::render::graph::tree::Pbr::record(const unsigned int kernel_index) noexcept
{
    for (auto* const cas : cascades) {
        cas->record(kernel_index);
    }

    for (auto& nnnn : nodes) {
        for (auto& nnn : nnnn.second) {
            for (auto& nn : nnn.second) {
                for (auto& n : nn.second) {
                    n.second->record(kernel_index);
                }
            }
        }
    }
}

void gearoenix::render::graph::tree::Pbr::submit() noexcept
{
    for (auto* cas : cascades) {
        cas->submit();
    }

    for (auto& nnnn : nodes) {
        for (auto& nnn : nnnn.second) {
            for (auto& nn : nnn.second) {
                for (auto& n : nn.second) {
                    n.second->submit();
                }
            }
        }
    }
}

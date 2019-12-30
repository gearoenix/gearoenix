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
#include "../node/rnd-gr-nd-unlit.hpp"
#include <memory>

#define GX_START_TASKS            \
    unsigned int task_number = 0; \
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();

#define GX_DO_TASK(expr)                                 \
    {                                                    \
        if (task_number == kernel_index) {               \
            expr;                                        \
        }                                                \
        task_number = (task_number + 1) % kernels_count; \
    }

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
    forward_pbr.refresh();
    unlit.refresh();
    nodes.clear();
    const auto& priorities_scenes = e->get_physics_engine()->get_scenes_camera_data().priority_ptr_scene;
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
                    const auto& models_lights = camera_data.second;
                    const auto& shadow_caster_directional_lights = models_lights.shadow_caster_directional_lights;
                    const auto& opaque_materials = models_lights.opaque_container_models;
                    const auto& transparent_materials = models_lights.transparent_container_models;
                    CameraData& camera_nodes = camera_priority_nodes[cam];
                    for (const auto& material_models : opaque_materials) {
                        switch (material_models.first) {
                        case material::Type::Pbr: {
                            auto* const n = forward_pbr.get_next([this] {
                                auto* const n = new node::ForwardPbr(e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                                n->set_render_target(e->get_main_render_target());
                                return n;
                            });
                            n->update();
                            n->set_scene(scn);
                            n->set_camera(cam);
                            n->set_directional_lights(&shadow_caster_directional_lights);
                            n->add_models(&material_models.second);
                            camera_nodes.opaques.forward_pbr = n;
                            break;
                        }
                        case material::Type::Unlit: {
                            auto* const n = unlit.get_next([this] {
                                auto* const n = new node::Unlit(e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                                n->set_render_target(e->get_main_render_target());
                                return n;
                            });
                            n->update();
                            n->set_camera(cam);
                            n->add_models(&material_models.second);
                            camera_nodes.opaques.unlit = n;
                            break;
                        }
                        default:
                            GXUNEXPECTED
                        }
                    }
                    node::ForwardPbr* previous_forward_pbr = nullptr;
                    node::Unlit* previous_unlit = nullptr;
                    camera_nodes.transparencies.clear();
                    for (const auto& dis : transparent_materials) {
                        for (const auto& material_models : dis.second) {
                            switch (material_models.first) {
                            case material::Type::Pbr: {
                                previous_unlit = nullptr;
                                if (previous_forward_pbr == nullptr) {
                                    previous_forward_pbr = forward_pbr.get_next([this] {
                                        auto* const n = new node::ForwardPbr(
                                            e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                                        n->set_render_target(e->get_main_render_target());
                                        return n;
                                    });
                                    camera_nodes.transparencies.push_back(previous_forward_pbr);
                                }
                                previous_forward_pbr->update();
                                previous_forward_pbr->set_scene(scn);
                                previous_forward_pbr->set_camera(cam);
                                previous_forward_pbr->set_directional_lights(&shadow_caster_directional_lights);
                                previous_forward_pbr->add_models(&material_models.second);
                                break;
                            }
                            case material::Type::Unlit: {
                                previous_forward_pbr = nullptr;
                                if (previous_unlit == nullptr) {
                                    previous_unlit = unlit.get_next([this] {
                                        auto* const n = new node::Unlit(
                                            e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                                        n->set_render_target(e->get_main_render_target());
                                        return n;
                                    });
                                    camera_nodes.transparencies.push_back(previous_unlit);
                                }
                                previous_unlit->update();
                                previous_unlit->set_camera(cam);
                                previous_unlit->add_models(&material_models.second);
                                break;
                            }
                            default:
                                GXUNEXPECTED
                            }
                        }
                    }
                    for (const auto& priority_lights : shadow_caster_directional_lights) {
                        const auto& lights = priority_lights;
                        for (const auto& light_cascades : lights.second) {
                            cascades.push_back(light_cascades.second);
                        }
                    }
                }
            }
        }
    }
}

void gearoenix::render::graph::tree::Pbr::record(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS

    for (auto* const cas : cascades) {
        cas->record(kernel_index);
    }

    for (const auto& priority_scenes : nodes) {
        for (const auto& scene_nodes : priority_scenes.second) {
            for (const auto& priority_cameras : scene_nodes.second) {
                for (const auto& camera_nodes : priority_cameras.second) {
                    const auto& opaques = camera_nodes.second.opaques;
                    if (opaques.forward_pbr != nullptr)
                        opaques.forward_pbr->record(kernel_index);
                    if (opaques.unlit != nullptr)
                        opaques.unlit->record(kernel_index);
                }
            }
        }
    }

    for (const auto& priority_scenes : nodes) {
        for (const auto& scene_nodes : priority_scenes.second) {
            for (const auto& priority_cameras : scene_nodes.second) {
                for (const auto& camera_nodes : priority_cameras.second) {
                    for (auto* const node : camera_nodes.second.transparencies) {
                        GX_DO_TASK(
                            if (node->get_render_node_type() == node::Type::ForwardPbr) {
                                reinterpret_cast<node::ForwardPbr*>(node)->record_continuously(kernel_index);
                            } else {
                                reinterpret_cast<node::Unlit*>(node)->record_continuously(kernel_index);
                            })
                    }
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

    for (auto& priority_scenes : nodes) {
        for (auto& scene_nodes : priority_scenes.second) {
            for (auto& priority_cameras : scene_nodes.second) {
                for (auto& camera_nodes : priority_cameras.second) {
                    const auto& opaques = camera_nodes.second.opaques;
                    if (opaques.forward_pbr != nullptr)
                        opaques.forward_pbr->submit();
                    if (opaques.unlit != nullptr)
                        opaques.unlit->submit();
                }
            }
        }
    }

    for (const auto& priority_scenes : nodes) {
        for (const auto& scene_nodes : priority_scenes.second) {
            for (const auto& priority_cameras : scene_nodes.second) {
                for (const auto& camera_nodes : priority_cameras.second) {
                    for (auto* const node : camera_nodes.second.transparencies) {
                        if (node->get_render_node_type() == node::Type::ForwardPbr) {
                            reinterpret_cast<node::ForwardPbr*>(node)->submit();
                        } else {
                            reinterpret_cast<node::Unlit*>(node)->submit();
                        }
                    }
                }
            }
        }
    }
}

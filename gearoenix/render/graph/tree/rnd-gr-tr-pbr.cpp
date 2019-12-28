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
#define CREATE_NODE(cls, x, np)                                                                          \
    {                                                                                                    \
        auto* const n = np.get_next([this] {                                                             \
            auto* const n = new node::cls(e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})); \
            n->set_render_target(e->get_main_render_target());                                           \
            return n;                                                                                    \
        });                                                                                              \
        n->set_scene(scn);                                                                               \
        n->set_camera(cam);                                                                              \
        n->set_directional_lights(&shadow_caster_directional_lights);                                    \
        n->add_models(&material_models.second);                                                          \
        n->update();                                                                                     \
        x = n;                                                                                           \
    }
#define CREATE_NODES(mm)                                                           \
    for (const auto& material_models : mm) {                                       \
        switch (material_models.first) {                                           \
        case material::Type::Pbr: {                                                \
            CREATE_NODE(ForwardPbr, camera_nodes.opaques.forward_pbr, forward_pbr) \
            break;                                                                 \
        }                                                                          \
        case material::Type::Unlit: {                                              \
            CREATE_NODE(Unlit, camera_nodes.opaques.unlit, unlit)                  \
            break;                                                                 \
        }                                                                          \
        default:                                                                   \
            GXUNEXPECTED                                                           \
        }                                                                          \
    }
                    CREATE_NODES(opaque_materials)
#undef CREATE_NODE
#define CREATE_NODE(cls, x, np)                                                                          \
    {                                                                                                    \
        auto* const n = previous_##np != nullptr ? previous_##np : np.get_next([this] {                  \
            auto* const n = new node::cls(e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})); \
            n->set_render_target(e->get_main_render_target());                                           \
            return n;                                                                                    \
        });                                                                                              \
        n->set_scene(scn);                                                                               \
        n->set_camera(cam);                                                                              \
        n->set_directional_lights(&shadow_caster_directional_lights);                                    \
        n->add_models(&material_models.second);                                                          \
        n->update();                                                                                     \
        x = n;                                                                                           \
        previous_##np = n;                                                                               \
    }
                    node::ForwardPbr* previous_forward_pbr = nullptr;
                    node::Unlit* previous_unlit = nullptr;
                    for (const auto& dis : transparent_materials) {
                        auto& transparent_nodes = camera_nodes.transparents[dis.first];
                        for (const auto& material_models : dis.second) {
                            switch (material_models.first) {
                            case material::Type::Pbr: {
                                previous_unlit = nullptr;
                                CREATE_NODE(ForwardPbr, transparent_nodes.forward_pbr, forward_pbr)
                                break;
                            }
                            case material::Type::Unlit: {
                                previous_forward_pbr = nullptr;
                                CREATE_NODE(Unlit, transparent_nodes.unlit, unlit)
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

    for (const auto& nnnn : nodes) {
        for (const auto& nnn : nnnn.second) {
            for (const auto& nn : nnn.second) {
                for (const auto& n : nn.second) {
                    const auto& cam = n.second.opaques;
                    cam.forward_pbr->record(kernel_index);
                    cam.unlit->record(kernel_index);
                }
            }
        }
    }

    for (const auto& nnnn : nodes) {
        for (const auto& nnn : nnnn.second) {
            for (const auto& nn : nnn.second) {
                for (const auto& n : nn.second) {
                    auto& distances = n.second.transparents;
                    for (const auto& distance_mdls : distances) {
                        GX_DO_TASK(distance_mdls.second.forward_pbr->record_continuously(kernel_index))
                        GX_DO_TASK(distance_mdls.second.unlit->record_continuously(kernel_index))
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

    for (auto& nnnn : nodes) {
        for (auto& nnn : nnnn.second) {
            for (auto& nn : nnn.second) {
                for (auto& n : nn.second) {
                    const auto& cam = n.second.opaques;
                    cam.forward_pbr->submit();
                    cam.unlit->submit();
                }
            }
        }
    }

    for (const auto& nnnn : nodes) {
        for (const auto& nnn : nnnn.second) {
            for (const auto& nn : nnn.second) {
                for (const auto& n : nn.second) {
                    auto& distances = n.second.transparents;
                    for (const auto& distance_mdls : distances) {
                        distance_mdls.second.forward_pbr->submit();
                        distance_mdls.second.unlit->submit();
                    }
                }
            }
        }
    }
}

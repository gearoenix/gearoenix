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
#include "../../skybox/rnd-sky-equirectangular.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../node/rnd-gr-nd-forward-pbr.hpp"
#include "../node/rnd-gr-nd-shadow-mapper.hpp"
#include "../node/rnd-gr-nd-skybox-equirectangular.hpp"
#include "../node/rnd-gr-nd-unlit.hpp"

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

void gearoenix::render::graph::tree::Pbr::CameraData::clear() noexcept {
        skyboxes.clear();
        opaques.forward_pbr = nullptr;
        opaques.unlit = nullptr;
        transparencies.clear();
}

void gearoenix::render::graph::tree::Pbr::update_skyboxes(const scene::Scene* const scn, const camera::Camera* const cam, CameraData& camera_nodes) noexcept
{
    node::SkyboxCube* previous_cube = nullptr;
    node::SkyboxEquirectangular* previous_equirectangular = nullptr;
    for (const auto& id_skybox : scn->get_skyboxs()) {
        const auto* const sky = id_skybox.second.get();
        if (!sky->get_enabled())
            continue;
        switch (sky->get_skybox_type()) {
        case skybox::Type::Equirectangular:
            previous_cube = nullptr;
            if (previous_equirectangular == nullptr) {
                previous_equirectangular = skybox_equirectangular.get_next([this] {
                    auto* const n = new node::SkyboxEquirectangular(e, GX_DEFAULT_IGNORED_END_CALLER);
                    n->set_render_target(e->get_main_render_target().get());
                    return n;
                });
                camera_nodes.skyboxes[sky->get_layer()].push_back(previous_equirectangular);
            }
            previous_equirectangular->update();
            previous_equirectangular->set_camera(cam);
            previous_equirectangular->add_sky(reinterpret_cast<const skybox::Equirectangular*>(sky));
            break;
        case skybox::Type::Cube:
            previous_equirectangular = nullptr;
            GXUNIMPLEMENTED
        default:
            GXUNEXPECTED
        }
    }
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
    forward_pbr.refresh();
//    skybox_cube.refresh();
    skybox_equirectangular.refresh();
    unlit.refresh();
    nodes.clear();
    cascades.clear();
    const auto& priorities_scenes = e->get_physics_engine()->get_sorted_scenes();
    for (const auto& priority_scenes : priorities_scenes) {
        const core::Real scene_priority = priority_scenes.first;
        auto &scene_priority_nodes = nodes[scene_priority];
        const scene::Scene *const scn = priority_scenes.second;
        auto &scene_nodes = scene_priority_nodes[scn];
        const auto &cameras = scn->get_cameras();
        for (const auto &id_camera : cameras) {
            auto *const cam = id_camera.second.get();
            const core::Real camera_priority = cam->get_layer();
            auto &camera_priority_nodes = scene_nodes[camera_priority];
            CameraData &camera_nodes = camera_priority_nodes[cam];
            camera_nodes.clear();
            update_skyboxes(scn, cam, camera_nodes);
            update_opaque(cam->get_seen_static_opaque_meshes(), scn, cam, camera_nodes);
            update_opaque(cam->get_seen_dynamic_opaque_meshes(), scn, cam, camera_nodes);
            update_transparent(cam->get_seen_transparent_meshes(), scn, cam, camera_nodes);
            for (auto &c : cam->get_cascades()) {
                cascades.push_back(&c);
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
                for (const auto& cam_camera_nodes : priority_cameras.second) {
                    const CameraData& camera_nodes = cam_camera_nodes.second;
                    for (const auto& skies : camera_nodes.skyboxes)
                        for (auto* const sky : skies.second)
                            GX_DO_TASK(sky->record_continuously(kernel_index))
                    const auto& opaques = camera_nodes.opaques;
                    if (opaques.forward_pbr != nullptr)
                        opaques.forward_pbr->record(kernel_index);
                    if (opaques.unlit != nullptr)
                        opaques.unlit->record(kernel_index);
                    for (auto* const node : camera_nodes.transparencies)
                        GX_DO_TASK(node->record_continuously(kernel_index))
                }
            }
        }
    }

    //    for (const auto& priority_scenes : nodes) {
    //        for (const auto& scene_nodes : priority_scenes.second) {
    //            for (const auto& priority_cameras : scene_nodes.second) {
    //                for (const auto& camera_nodes : priority_cameras.second) {
    //                    for (auto* const node : camera_nodes.second.transparencies) {
    //                        GX_DO_TASK(switch (node->get_render_node_type()) {
    //                            case node::Type::ForwardPbr:
    //                                reinterpret_cast<node::ForwardPbr*>(node)->record_continuously(kernel_index);
    //                                break;
    //                            case node::Type::Unlit:
    //                                reinterpret_cast<node::Unlit*>(node)->record_continuously(kernel_index);
    //                                break;
    //                            default:
    //                                GXUNEXPECTED
    //                        })
    //                    }
    //                }
    //            }
    //        }
    //    }
}

void gearoenix::render::graph::tree::Pbr::submit() noexcept
{
    for (auto* cas : cascades) {
        cas->submit();
    }

    for (auto& priority_scenes : nodes) {
        for (auto& scene_nodes : priority_scenes.second) {
            for (auto& priority_cameras : scene_nodes.second) {
                for (auto& cam_camera_nodes : priority_cameras.second) {
                    const CameraData& camera_nodes = cam_camera_nodes.second;
                    for (const auto& skies : camera_nodes.skyboxes)
                        for (auto* const sky : skies.second)
                            sky->submit();
                    const auto& opaques = camera_nodes.opaques;
                    if (opaques.forward_pbr != nullptr)
                        opaques.forward_pbr->submit();
                    if (opaques.unlit != nullptr)
                        opaques.unlit->submit();
                    for (auto* const node : camera_nodes.transparencies)
                        node->submit();
                }
            }
        }
    }

    //    for (const auto& priority_scenes : nodes) {
    //        for (const auto& scene_nodes : priority_scenes.second) {
    //            for (const auto& priority_cameras : scene_nodes.second) {
    //                for (const auto& camera_nodes : priority_cameras.second) {
    //                    for (auto* const node : camera_nodes.second.transparencies) {
    //                        switch (node->get_render_node_type()) {
    //                        case node::Type::ForwardPbr:
    //                            reinterpret_cast<node::ForwardPbr*>(node)->submit();
    //                            break;
    //                        case node::Type::Unlit:
    //                            reinterpret_cast<node::Unlit*>(node)->submit();
    //                            break;
    //                        default:
    //                            GXUNEXPECTED
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
}

void gearoenix::render::graph::tree::Pbr::update_opaque(
        const std::vector<std::tuple< material::Type, model::Model*, model::Mesh*>>& seen_meshes,
        const scene::Scene*const scn, const camera::Camera*const cam, CameraData& camera_nodes
        ) noexcept {
    node::ForwardPbr * fwd = camera_nodes.opaques.forward_pbr;
    if( fwd == nullptr) {
        fwd = forward_pbr.get_next([this] {
            return new node::ForwardPbr(e, GX_DEFAULT_IGNORED_END_CALLER);
        });
        fwd->update();
        fwd->set_scene(scn);
        fwd->set_camera(cam);

        const auto& cs = cam->get_cascades();
        for(auto& cascade: cs ) {
            fwd->add_cascade(&cascade);
        }
        camera_nodes.opaques.forward_pbr = fwd;
    }

    node::Unlit *unl = camera_nodes.opaques.unlit;
    if(unl == nullptr) {
        unl = unlit.get_next([this] {
            return new node::Unlit(e, GX_DEFAULT_IGNORED_END_CALLER);
        });
        unl->update();
        unl->set_camera(cam);
        camera_nodes.opaques.unlit = unl;
    }

    for (const auto [material_id, model_ptr, mesh_ptr] : seen_meshes) {
        switch (material_id) {
            case material::Type::Pbr: {
                fwd->add_mesh(std::make_pair(model_ptr, mesh_ptr));
                break;
            }
            case material::Type::Unlit: {
                unl->add_mesh(std::make_pair(model_ptr, mesh_ptr));
                break;
            }
            default: GXUNEXPECTED
        }
    }
}

void gearoenix::render::graph::tree::Pbr::update_transparent(
        const std::vector<std::tuple<core::Real, material::Type, model::Model *, model::Mesh *>> &seen_meshes,
        const scene::Scene *scn, const camera::Camera *cam, CameraData &camera_nodes) noexcept {
    const auto& cs = cam->get_cascades();
    node::ForwardPbr *fwd = nullptr;
    node::Unlit *unl = nullptr;
    for (const auto [dis, mat_type, mdl, msh] : seen_meshes) {
        switch (mat_type) {
            case material::Type::Pbr: {
                unl = nullptr;
                if (fwd == nullptr) {
                    fwd = forward_pbr.get_next([this] {
                        auto *const n = new node::ForwardPbr(
                                e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                        n->set_render_target(e->get_main_render_target().get());
                        return n;
                    });
                    camera_nodes.transparencies.push_back(fwd);
                    fwd->update();
                    fwd->set_scene(scn);
                    fwd->set_camera(cam);
                    for(auto& c: cs) {
                        fwd->add_cascade(&c);
                    }
                }
                fwd->add_mesh({ mdl, msh });
                break;
            }
            case material::Type::Unlit: {
                fwd = nullptr;
                if (unl == nullptr) {
                    unl = unlit.get_next([this] {
                        auto *const n = new node::Unlit(
                                e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                        n->set_render_target(e->get_main_render_target().get());
                        return n;
                    });
                    camera_nodes.transparencies.push_back(unl);
                    unl->update();
                    unl->set_camera(cam);
                }
                unl->add_mesh({mdl, msh});
                break;
            }
            default: GXUNEXPECTED
        }
        (void)dis;
    }
}

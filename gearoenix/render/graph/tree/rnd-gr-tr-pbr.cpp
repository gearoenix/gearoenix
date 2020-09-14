#include "rnd-gr-tr-pbr.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../physics/phs-engine.hpp"
#include "../../../system/sys-app.hpp"
#include "../../camera/rnd-cmr-perspective.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-cascade-info.hpp"
#include "../../light/rnd-lt-directional.hpp"
#include "../../reflection/rnd-rfl-runtime.hpp"
#include "../../scene/rnd-scn-manager.hpp"
#include "../../skybox/rnd-sky-cube.hpp"
#include "../../skybox/rnd-sky-equirectangular.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../node/rnd-gr-nd-forward-pbr.hpp"
#include "../node/rnd-gr-nd-irradiance-convoluter.hpp"
#include "../node/rnd-gr-nd-mipmap-generator.hpp"
#include "../node/rnd-gr-nd-radiance-convoluter.hpp"
#include "../node/rnd-gr-nd-shadow-mapper.hpp"
#include "../node/rnd-gr-nd-skybox-cube.hpp"
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

void gearoenix::render::graph::tree::Pbr::CameraData::clear() noexcept
{
    skyboxes.clear();
    opaques.forward_pbr = nullptr;
    opaques.unlit = nullptr;
    transparencies.clear();
}

void gearoenix::render::graph::tree::Pbr::update_camera(
    const scene::Scene* const scn,
    camera::Camera* const cam,
    graph::tree::Pbr::CameraData& camera_nodes) noexcept
{
    camera_nodes.clear();
    update_skyboxes(scn, cam, camera_nodes);
    update_opaque(cam->get_seen_static_opaque_meshes(), scn, cam, camera_nodes);
    update_opaque(cam->get_seen_dynamic_opaque_meshes(), scn, cam, camera_nodes);
    update_transparent(cam->get_seen_transparent_meshes(), scn, cam, camera_nodes);
    for (auto& c : cam->get_cascades()) {
        cascades.push_back(&c);
    }
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
                    auto* const n = new node::SkyboxEquirectangular("pbr-sky-equ", e, GX_DEFAULT_IGNORED_END_CALLER);
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
            if (previous_cube == nullptr) {
                previous_cube = skybox_cube.get_next([this] {
                    auto* const n = new node::SkyboxCube("pbr-sky-cube", e, GX_DEFAULT_IGNORED_END_CALLER);
                    n->set_render_target(e->get_main_render_target().get());
                    return n;
                });
                camera_nodes.skyboxes[sky->get_layer()].push_back(previous_cube);
            }
            previous_cube->update();
            previous_cube->set_camera(cam);
            previous_cube->add_sky(reinterpret_cast<const skybox::Cube*>(sky));
            break;
        default:
            GX_UNEXPECTED
        }
    }
}

void gearoenix::render::graph::tree::Pbr::update_runtime_reflection(const scene::Scene* const scn) noexcept
{
    const auto& runtime_reflections = scn->get_runtime_reflections();
    for (const auto& id_rtr : runtime_reflections) {
        auto* const rtr = id_rtr.second.get();
        if (!rtr->get_enabled())
            continue;
        switch (rtr->get_state()) {
        case reflection::Runtime::State::EnvironmentCubeRender: {
            if (rtr->get_state_environment_face() == 0) {
                runtime_reflections_data.emplace(rtr, RuntimeReflectionData(rtr->get_radiance_convoluters()[0].size()));
            }
            auto& rtr_nodes = runtime_reflections_data.find(rtr)->second;
            const auto& cam = rtr->get_cameras()[rtr->get_state_environment_face()];
            auto& rtr_face_nodes = rtr_nodes.faces[rtr->get_state_environment_face()];
            rtr_face_nodes.cam = cam.get();
            update_camera(scn, rtr_face_nodes.cam, rtr_face_nodes.camera_data);
            break;
        }
        case reflection::Runtime::State::EnvironmentCubeMipMap: {
            auto& rtr_nodes = runtime_reflections_data.find(rtr)->second;
            rtr_nodes.environment_mipmap_generator = rtr->get_environment_mipmap_generator().get();
            rtr_nodes.environment_mipmap_generator->update();
            rtr_nodes.irradiance_mipmap_generator = rtr->get_irradiance_mipmap_generator().get();
            rtr_nodes.irradiance_mipmap_generator->update();
            break;
        }
        case reflection::Runtime::State::IrradianceFace: {
            const auto fi = rtr->get_state_irradiance_face();
            auto& irradiance = runtime_reflections_data.find(rtr)->second.faces[fi].irradiance;
            irradiance = rtr->get_irradiance_convoluters()[fi].get();
            irradiance->update();
            break;
        }
        case reflection::Runtime::State::IrradianceMipMap: {
            auto& rtr_nodes = runtime_reflections_data.find(rtr)->second;
            rtr_nodes.irradiance_mipmap_generator = rtr->get_irradiance_mipmap_generator().get();
            rtr_nodes.irradiance_mipmap_generator->update();
            break;
        }
        case reflection::Runtime::State::RadianceFaceLevel: {
            auto& rtr_nodes = runtime_reflections_data.find(rtr)->second;
            const auto& radiances = rtr->get_radiance_convoluters();
            const auto fi = rtr->get_state_radiance_face();
            auto& rtr_face_nodes = rtr_nodes.faces[fi];
            const auto& face_radiances = radiances[fi];
            const auto li = rtr->get_state_radiance_level();
            auto* const radiance = face_radiances[li].get();
            rtr_face_nodes.radiances[li] = radiance;
            radiance->update();
            break;
        }
        default:
            return;
        }
    }
}

void gearoenix::render::graph::tree::Pbr::update_opaque(
    const std::vector<std::tuple<double, material::Type, model::Model*, model::Mesh*>>& seen_meshes,
    const scene::Scene* const scn, const camera::Camera* const cam, CameraData& camera_nodes) noexcept
{
    node::ForwardPbr* fwd = camera_nodes.opaques.forward_pbr;
    if (fwd == nullptr) {
        fwd = forward_pbr.get_next([this] {
            return new node::ForwardPbr("pbr-fwd-pbr", e, GX_DEFAULT_IGNORED_END_CALLER);
        });
        fwd->update();
        fwd->set_scene(scn);
        fwd->set_camera(cam);

        const auto& cs = cam->get_cascades();
        for (auto& cascade : cs) {
            fwd->add_cascade(&cascade);
        }
        camera_nodes.opaques.forward_pbr = fwd;
    }

    node::Unlit* unl = camera_nodes.opaques.unlit;
    if (unl == nullptr) {
        unl = unlit.get_next([this] {
            return new node::Unlit("pbr-tree-unlit", e, GX_DEFAULT_IGNORED_END_CALLER);
        });
        unl->update();
        unl->set_camera(cam);
        camera_nodes.opaques.unlit = unl;
    }

    for (const auto& [dis, material_id, model_ptr, mesh_ptr] : seen_meshes) {
        switch (material_id) {
        case material::Type::Pbr: {
            fwd->add_mesh(std::make_pair(model_ptr, mesh_ptr));
            break;
        }
        case material::Type::Unlit: {
            unl->add_mesh(std::make_pair(model_ptr, mesh_ptr));
            break;
        }
        default:
            GX_UNEXPECTED
        }
    }
}

void gearoenix::render::graph::tree::Pbr::update_transparent(
    const std::vector<std::tuple<double, material::Type, model::Model*, model::Mesh*>>& seen_meshes,
    const scene::Scene* scn, const camera::Camera* cam, CameraData& camera_nodes) noexcept
{
    const auto& cs = cam->get_cascades();
    node::ForwardPbr* fwd = nullptr;
    node::Unlit* unl = nullptr;
    for (const auto& [dis, mat_type, mdl, msh] : seen_meshes) {
        switch (mat_type) {
        case material::Type::Pbr: {
            unl = nullptr;
            if (fwd == nullptr) {
                fwd = forward_pbr.get_next([this] {
                    auto* const n = new node::ForwardPbr(
                        "pbr-tree-pbr", e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                    n->set_render_target(e->get_main_render_target().get());
                    return n;
                });
                camera_nodes.transparencies.push_back(fwd);
                fwd->update();
                fwd->set_scene(scn);
                fwd->set_camera(cam);
                for (auto& c : cs) {
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
                    auto* const n = new node::Unlit(
                        "pbr-tree-unlit", e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
                    n->set_render_target(e->get_main_render_target().get());
                    return n;
                });
                camera_nodes.transparencies.push_back(unl);
                unl->update();
                unl->set_camera(cam);
            }
            unl->add_mesh({ mdl, msh });
            break;
        }
        default:
            GX_UNEXPECTED
        }
        (void)dis;
    }
}

void gearoenix::render::graph::tree::Pbr::record_runtime_reflection(
    const scene::Scene* const scn, unsigned int& task_number, const unsigned int kernel_index,
    const unsigned int kernels_count) noexcept
{
    const auto& runtime_reflections = scn->get_runtime_reflections();
    for (const auto& id_rtr : runtime_reflections) {
        auto* const rtr = id_rtr.second.get();
        if (!rtr->get_enabled())
            continue;
        auto id_rtr_data = runtime_reflections_data.find(rtr);
        if (runtime_reflections_data.end() == id_rtr_data)
            continue;
        auto& runtime_reflection = id_rtr_data->second;
        switch (rtr->get_state()) {
        case reflection::Runtime::State::EnvironmentCubeRender: {
            const auto& face = runtime_reflection.faces[rtr->get_state_environment_face()];
            for (const auto& skies : face.camera_data.skyboxes) {
                for (auto* const sky : skies.second) {
                    GX_DO_TASK(sky->record_continuously(kernel_index))
                }
            }
            const auto& opaques = face.camera_data.opaques;
            if (opaques.forward_pbr != nullptr)
                opaques.forward_pbr->record(kernel_index);
            if (opaques.unlit != nullptr)
                opaques.unlit->record(kernel_index);
            for (auto* const node : face.camera_data.transparencies) {
                GX_DO_TASK(node->record_continuously(kernel_index))
            }
            break;
        }
        case reflection::Runtime::State::EnvironmentCubeMipMap: {
            GX_DO_TASK(runtime_reflection.environment_mipmap_generator->record_continuously(kernel_index))
            break;
        }
        case reflection::Runtime::State::IrradianceFace: {
            GX_DO_TASK(runtime_reflection.faces[rtr->get_state_irradiance_face()].irradiance->record_continuously(kernel_index))
            break;
        }
        case reflection::Runtime::State::IrradianceMipMap: {
            GX_DO_TASK(runtime_reflection.irradiance_mipmap_generator->record_continuously(kernel_index))
            break;
        }
        case reflection::Runtime::State::RadianceFaceLevel: {
            GX_DO_TASK(runtime_reflection.faces[rtr->get_state_radiance_face()].radiances[rtr->get_state_radiance_level()]->record_continuously(kernel_index))
            break;
        }
        default:
            return;
        }
    }
}

void gearoenix::render::graph::tree::Pbr::submit_camera_data(const CameraData& camera_data) const noexcept
{
    for (const auto& skies : camera_data.skyboxes)
        for (auto* const sky : skies.second)
            sky->submit();
    const auto& opaques = camera_data.opaques;
    if (opaques.forward_pbr != nullptr)
        opaques.forward_pbr->submit();
    if (opaques.unlit != nullptr)
        opaques.unlit->submit();
    for (auto* const node : camera_data.transparencies)
        node->submit();
}

void gearoenix::render::graph::tree::Pbr::submit_runtime_reflections(const scene::Scene* const scn) noexcept
{
    const auto& runtime_reflections = scn->get_runtime_reflections();
    for (const auto& id_rtr : runtime_reflections) {
        auto* const rtr = id_rtr.second.get();
        if (!rtr->get_enabled())
            continue;
        auto id_rtr_data = runtime_reflections_data.find(rtr);
        if (runtime_reflections_data.end() != id_rtr_data) {
            auto& runtime_reflection = id_rtr_data->second;
            switch (rtr->get_state()) {
            case reflection::Runtime::State::EnvironmentCubeRender:
                submit_camera_data(runtime_reflection.faces[rtr->get_state_environment_face()].camera_data);
                break;
            case reflection::Runtime::State::EnvironmentCubeMipMap:
                runtime_reflection.environment_mipmap_generator->submit();
                break;
            case reflection::Runtime::State::IrradianceFace:
                runtime_reflection.faces[rtr->get_state_irradiance_face()].irradiance->submit();
                break;
            case reflection::Runtime::State::IrradianceMipMap:
                runtime_reflection.irradiance_mipmap_generator->submit();
                break;
            case reflection::Runtime::State::RadianceFaceLevel: {
                auto* const radiance_node = runtime_reflection.faces[rtr->get_state_radiance_face()].radiances[rtr->get_state_radiance_level()];
                if (nullptr != radiance_node)
                    radiance_node->submit();
                break;
            }
            default:
                break;
            }
        }
        rtr->update_state();
    }
}

gearoenix::render::graph::tree::Pbr::Pbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
    : Tree(e)
    , in_weak_hardware(engine::Type::OpenGLES2 == e->get_engine_type())
{
}

gearoenix::render::graph::tree::Pbr::~Pbr() noexcept
{
    GXLOGD("Pbr render tree deleted.")
}

void gearoenix::render::graph::tree::Pbr::update() noexcept
{
    forward_pbr.refresh();
    skybox_cube.refresh();
    skybox_equirectangular.refresh();
    unlit.refresh();
    nodes.clear();
    cascades.clear();
    const auto& priorities_scenes = e->get_physics_engine()->get_sorted_scenes();
    for (const auto& priority_scenes : priorities_scenes) {
        const double scene_priority = priority_scenes.first;
        auto& scene_priority_nodes = nodes[scene_priority];
        const scene::Scene* const scn = priority_scenes.second.get();
        auto& scene_nodes = scene_priority_nodes[scn];
        update_runtime_reflection(scn);
        const auto& cameras = scn->get_cameras();
        for (const auto& id_camera : cameras) {
            auto* const cam = id_camera.second.get();
            const double camera_priority = cam->get_layer();
            auto& camera_priority_nodes = scene_nodes.cameras[camera_priority];
            CameraData& camera_nodes = camera_priority_nodes[cam];
            update_camera(scn, cam, camera_nodes);
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
        for ([[maybe_unused]] const auto& [scn, scene_data] : priority_scenes.second) {
            record_runtime_reflection(scn, task_number, kernel_index, kernels_count);
            for (const auto& priority_cameras : scene_data.cameras) {
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
}

void gearoenix::render::graph::tree::Pbr::submit() noexcept
{
    for (auto* cas : cascades) {
        cas->submit();
    }

    for (auto& priority_scenes : nodes) {
        for (const auto& [scn, scene_data] : priority_scenes.second) {
            submit_runtime_reflections(scn);
            for ([[maybe_unused]] const auto& [priority, cameras] : scene_data.cameras) {
                for ([[maybe_unused]] const auto& [cam, camera_data] : cameras) {
                    submit_camera_data(camera_data);
                }
            }
        }
    }
}

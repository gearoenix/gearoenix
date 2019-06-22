#include "rnd-gr-tr-pbr.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../physics/phs-engine.hpp"
#include "../../../system/sys-app.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-cascade-info.hpp"
#include "../../light/rnd-lt-directional.hpp"
#include "../../scene/rnd-scn-manager.hpp"
#include "../../scene/rnd-scn-scene.hpp"
#include "../node/rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "../node/rnd-gr-nd-shadow-mapper.hpp"
#include <memory>

gearoenix::render::graph::tree::Pbr::Pbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Tree(e)
    , in_weak_hardware(engine::Type::OPENGL_ES2 == e->get_engine_type_id())
{
    fwddirshd = std::make_shared<node::ForwardPbrDirectionalShadow>(e, call);
    fwddirshd->set_render_target(e->get_main_render_target());
}

void gearoenix::render::graph::tree::Pbr::update() noexcept
{
    const auto& visible_models = e->get_physics_engine()->get_visible_models();
    for (const auto& scene_camera : visible_models) {
        const auto& cameras_data = scene_camera.second;
        for (const auto& camera_data : cameras_data) {
            const auto& lights_cascades_info = camera_data.second.second;
            for (const auto& light_cascades_info : lights_cascades_info) {
                auto& cds = light_cascades_info.second->get_cascades_data();
                for (auto& c : cds) {
                    auto& shm = c.shadow_mapper;
                    core::graph::Node::connect(shm, 0, fwddirshd, node::ForwardPbrDirectionalShadow::shadow_map_index);
                    fwddirshd->set_input_texture(shm->get_output_texture(0), node::ForwardPbrDirectionalShadow::shadow_map_index);
                }
            }
        }
    }
    fwddirshd->update();
}

void gearoenix::render::graph::tree::Pbr::record(const unsigned int kernel_index) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    unsigned int task_number = 0;
    const auto& visible_models = e->get_physics_engine()->get_visible_models();
    unsigned int scene_number = 0;
#define GX_DO_TASK(expr)               \
    if (task_number == kernel_index) { \
        expr;                          \
    }                                  \
    task_number = (task_number + 1) % kernels_count
    for (const auto& scene_camera : visible_models) {
        const auto* scn = scene_camera.first;
        const auto& lights = scn->get_lights();
        for (const auto& camera_models : scene_camera.second) {
            const auto* const cam = camera_models.first;
            const auto& models = camera_models.second.first;
            const auto& lights_cascades_info = camera_models.second.second;
            for (const auto& light_cascades_info : lights_cascades_info)
                light_cascades_info.second->record(kernel_index);
            for (const auto& id_light : lights) {
                if (!id_light.second->is_shadower())
                    continue;
                auto dirlt = dynamic_cast<const light::Directional*>(id_light.second.get());
                if (dirlt != nullptr) {
                    for (const auto* const model : models) {
                        GX_DO_TASK(fwddirshd->record(scn, cam, dirlt, model, kernel_index));
                    }
                    continue; /// This is for future
                }
                // const std::shared_ptr<light::Point> pntlt = std::dynamic_pointer_cast<light::Point>(id_light.second);
                /// like ...
            }
        }
        ++scene_number;
    }
#undef GX_DO_TASK
}

void gearoenix::render::graph::tree::Pbr::submit() noexcept
{
    const auto& visible_models = e->get_physics_engine()->get_visible_models();
    for (const auto& scene_camera : visible_models) {
        const auto& cameras_data = scene_camera.second;
        for (const auto& camera_data : cameras_data) {
            const auto& lights_cascades_info = camera_data.second.second;
            for (const auto& light_cascades_info : lights_cascades_info) {
                auto* cas_inf = light_cascades_info.second;
                cas_inf->submit();
                fwddirshd->set_cascades(cas_inf->get_cascades_data());
                fwddirshd->submit();
            }
        }
    }
}

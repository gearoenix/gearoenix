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
#include "../node/rnd-gr-nd-forward-pbr.hpp"
#include "../node/rnd-gr-nd-shadow-mapper.hpp"
#include <memory>

gearoenix::render::graph::tree::Pbr::Pbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Tree(e)
    , in_weak_hardware(engine::Type::OPENGL_ES2 == e->get_engine_type_id())
{
}

void gearoenix::render::graph::tree::Pbr::update() noexcept
{
	fwd.refresh();
    const auto& visible_models = e->get_physics_engine()->get_visible_models();
    for (const auto& scene_camera : visible_models) {
		const scene::Scene* const scn = scene_camera.first;
        const auto& cameras_data = scene_camera.second;
        for (const auto& camera_data : cameras_data) {
			const camera::Camera*const cam = camera_data.first;
			node::ForwardPbr *const n = fwd.get_next([this] {
				auto *const n = new node::ForwardPbr(
					e,
					core::sync::EndCaller<core::sync::EndCallerIgnore>([] {}));
				n->set_render_target(e->get_main_render_target());
				return n;
			});

			n->set_scene(scn);
			n->set_camera(cam);
			const auto& models_lights = camera_data.second;
			n->set_directional_lights(&models_lights.second);
			n->set_seen_models(&models_lights.first);
            /*const auto& lights_cascades_info = camera_data.second.second;
            for (const auto& light_cascades_info : lights_cascades_info) {
                auto& cds = light_cascades_info.second->get_cascades_data();
                for (const light::CascadeInfo::PerCascade& c : cds) {
                    graph::node::ShadowMapper*const shm = c.shadow_mapper;
                    core::graph::Node::connect(shm, 0, fwddirshd, node::ForwardPbr::SHADOW_MAP_INDEX);
                    fwddirshd->set_input_texture(shm->get_output_texture(0), node::ForwardPbr::SHADOW_MAP_INDEX);
                }
            }*/
			n->update();
        }
    }
}

void gearoenix::render::graph::tree::Pbr::record(const unsigned int kernel_index) noexcept
{
//    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
//    unsigned int task_number = 0;
//    const auto& visible_models = e->get_physics_engine()->get_visible_models();
//    unsigned int scene_number = 0;
//#define GX_DO_TASK(expr) {                             \
//    if (task_number == kernel_index) { expr; }         \
//    task_number = (task_number + 1) % kernels_count; }
//    for (const auto& scene_camera : visible_models) {
//        const auto* scn = scene_camera.first;
//        const auto& lights = scn->get_lights();
//        for (const auto& camera_models : scene_camera.second) {
//            const auto* const cam = camera_models.first;
//            const auto& models = camera_models.second.first;
//            const auto& lights_cascades_info = camera_models.second.second;
//            for (const auto& light_cascades_info : lights_cascades_info) {
//                auto* cas = light_cascades_info.second;
//                auto* dirlt = light_cascades_info.first;
//                cas->record(kernel_index);
//                for (const auto* const m : models) {
//                    GX_DO_TASK(fwddirshd->record(scn, cam, dirlt, m, cas, kernel_index))
//                }
//            }
//            // TODO: Camera independent lights handles in follow
//            // for (const auto& id_light : lights) {
//            //    if (!id_light.second->is_shadower())
//            //        continue;
//            //    auto pntlt = dynamic_cast<const light::Point *>(id_light.second.get());
//            //    if (pntlt != nullptr) { }
//            // }
//        }
//        ++scene_number;
//    }
//#undef GX_DO_TASK
	for (graph::node::ForwardPbr& n : fwd)
	{
		n.record(kernel_index);
	}
}

void gearoenix::render::graph::tree::Pbr::submit() noexcept
{
	for (graph::node::ForwardPbr& n : fwd)
	{
		n.submit();
	}
}

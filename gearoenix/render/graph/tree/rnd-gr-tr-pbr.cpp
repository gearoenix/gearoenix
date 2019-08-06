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
            
			n->update();
        }
    }
}

void gearoenix::render::graph::tree::Pbr::record(const unsigned int kernel_index) noexcept
{
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

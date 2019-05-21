#include <memory>

#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../physics/phs-engine.hpp"
#include "../../../system/sys-app.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-directional.hpp"
#include "../../scene/rnd-scn-manager.hpp"
#include "../../scene/rnd-scn-scene.hpp"
#include "../node/rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "rnd-gr-tr-pbr.hpp"

gearoenix::render::graph::tree::Pbr::Pbr(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Tree(e)
    , in_weak_hardware(engine::Type::OPENGL_ES2 == e->get_engine_type_id())
{
    fwddirshd = std::make_shared<node::ForwardPbrDirectionalShadow>(e, call);
    fwddirshd->set_render_target(e->get_main_render_target());
}

void gearoenix::render::graph::tree::Pbr::update() noexcept
{
    fwddirshd->update();
}

void gearoenix::render::graph::tree::Pbr::record(const unsigned int kernel_index) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    unsigned int task_number = 0;
    const physics::Engine::SceneCameraModelSet& visible_models = e->get_physics_engine()->get_visible_models();
    unsigned int scene_number = 0;
#define GXDOTASK(expr)                 \
    if (task_number == kernel_index) { \
        expr;                          \
    }                                  \
    task_number = (task_number + 1) % kernels_count
    for (const std::pair<const std::shared_ptr<scene::Scene>, physics::Engine::CameraModelSet>& scene_camera : visible_models) {
        const std::shared_ptr<scene::Scene>& scn = scene_camera.first;
        const std::map<core::Id, std::shared_ptr<light::Light>> lights = scn->get_lights();
        for (const std::pair<camera::Camera* const, physics::Engine::ModelSet>& camera_models : scene_camera.second) {
            const camera::Camera* const cam = camera_models.first;
            const std::set<model::Model*>& models = camera_models.second;
            for (const std::pair<const core::Id, std::shared_ptr<light::Light>>& id_light : lights) {
                if (!id_light.second->is_shadower())
                    continue;
                const std::shared_ptr<light::Directional> dirlt = std::dynamic_pointer_cast<light::Directional>(id_light.second);
                if (dirlt != nullptr) {
                    for (const model::Model* const model : models) {
                        GXDOTASK(fwddirshd->record(scn.get(), cam, dirlt.get(), model, kernel_index));
                    }
                    continue; /// This is for future
                }
                // const std::shared_ptr<light::Point> pntlt = std::dynamic_pointer_cast<light::Point>(id_light.second);
                /// like ...
            }
        }
        ++scene_number;
    }
#undef GXDOTASK
}

void gearoenix::render::graph::tree::Pbr::submit() noexcept
{
    fwddirshd->submit();
}

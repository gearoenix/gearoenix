#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/gltf/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <ranges>

template <typename T>
using GxEndCaller = gearoenix::core::job::EndCaller<T>;

typedef gearoenix::core::ecs::Entity GxEntity;
typedef gearoenix::core::ecs::EntityPtr GxEntityPtr;
typedef gearoenix::core::Object GxObject;
typedef gearoenix::physics::constraint::Manager GxConstraintManager;
typedef gearoenix::physics::Transformation GxTransform;
typedef gearoenix::platform::stream::Path GxPath;
typedef gearoenix::render::camera::Camera GxCamera;
typedef std::vector<GxEntityPtr> GxEntityPtrs;

struct GameApp final : gearoenix::core::Application {
private:
    GxEntityPtrs scene_entities;

public:
    GameApp()
    {
        gearoenix::render::gltf::load(
            GxPath::create_asset("sample.glb"),
            GxEndCaller<std::vector<GxEntityPtr>>([this](std::vector<GxEntityPtr>&& in_scene_entities) -> void {
                scene_entities = std::move(in_scene_entities);
                for (auto& e : scene_entities) {
                    if (find_main_camera(e.get())) {
                        break;
                    }
                }
            }));
    }

    [[nodiscard]] bool find_main_camera(GxEntity* const e)
    {
        if (const auto* const c = e->get_component<GxCamera>(); c && c->get_usage() == GxCamera::Usage::Main) {
            set_camera(e);
            return true;
        }
        for (auto& c : e->get_children() | std::views::values) {
            if (find_main_camera(c.get())) {
                return true;
            }
        }
        return false;
    }

    void set_camera(const GxEntity* const e)
    {
        (void)GxConstraintManager::get().create_jet_controller(
            e->get_object_name() + "-controller",
            e->get_component_shared_ptr<GxTransform>(),
            scene_entities[0].get());

        scene_entities[0]->add_to_world();
    }
};

GEAROENIX_START(GameApp)
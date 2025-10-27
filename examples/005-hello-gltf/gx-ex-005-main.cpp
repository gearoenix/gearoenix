#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gltf/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
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
typedef gearoenix::render::camera::Manager GxCamManager;
typedef gearoenix::render::engine::Engine GxRndEngine;
typedef gearoenix::render::light::Directional GxDirLight;
typedef gearoenix::render::light::ShadowCasterDirectional GxShadowDirLight;
typedef gearoenix::render::light::Manager GxLightMgr;

typedef std::vector<GxEntityPtr> GxEntityPtrs;

typedef gearoenix::core::job::EndCaller<GxEntityPtr> GxEntityEndCaller;

struct GameApp final : gearoenix::core::Application {
private:
    GxEntityPtrs scene_entities;

public:
    GameApp()
    {
        gearoenix::render::gltf::load(
            // GxPath::create_asset("sample.glb"),
            // GxPath::create_absolute("../../../../../submodules/glTF-Sample-Assets/Models/Avocado/glTF/Avocado.gltf"),
            // GxPath::create_absolute("../../../../../submodules/glTF-Sample-Assets/Models/FlightHelmet/glTF/FlightHelmet.gltf"),
            GxPath::create_absolute("../../../../../submodules/glTF-Sample-Assets/Models/RiggedSimple/glTF/RiggedSimple.gltf"),
            GxEndCaller<std::vector<GxEntityPtr>>([this](std::vector<GxEntityPtr>&& in_scene_entities) {
                scene_entities = std::move(in_scene_entities);
                GX_ASSERT_D(!scene_entities.empty()); // No scene entities found.
                bool camera_found = false;
                bool light_found = false;
                for (auto& e : scene_entities) {
                    if (!light_found && find_light(e.get())) {
                        light_found = true;
                    }
                    if (!camera_found && find_main_camera(e.get())) {
                        camera_found = true;
                    }
                }
                if (!light_found) {
                    create_light();
                }
                if (!camera_found) {
                    create_camera();
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

    void create_camera()
    {
        GxCamManager::get().build("camera", scene_entities[0].get(), GxEntityEndCaller([this](GxEntityPtr&& e) {
            e->get_component<GxTransform>()->local_look_at({ 10.0, 10.0, 10.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 });
            set_camera(e.get());
        }));
    }

    void set_camera(const GxEntity* const e)
    {
        (void)GxConstraintManager::get().create_jet_controller(
            e->get_object_name() + "-controller", e->get_component_shared_ptr<GxTransform>(),
            scene_entities[0].get());

        scene_entities[0]->add_to_world();
    }

    [[nodiscard]] static bool find_light(GxEntity* const e)
    {
        if (const auto* const l = e->get_component<GxDirLight>(); l) {
            return true;
        }

        if (const auto* const l = e->get_component<GxShadowDirLight>(); l) {
            return true;
        }

        for (auto& c : e->get_children() | std::views::values) {
            if (find_light(c.get())) {
                return true;
            }
        }
        return false;
    }

    void create_light()
    {
        GxLightMgr::get().build_shadow_caster_directional(
            "directional-light", scene_entities[0].get(), 1024, 20.0f, 1.0f, 50.0f,
            GxEntityEndCaller([this](GxEntityPtr&& l) { set_light(std::move(l)); }));
    }

    static void set_light(GxEntityPtr&& light_entity)
    {
        auto* const light = light_entity->get_component<GxShadowDirLight>();
        light->get_shadow_transform()->local_look_at({ 10.0, 10.0, 10.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 });
        light->colour = { 2.0f, 2.0f, 2.0f };
    }

    void update() override
    {
        GxRndEngine::get().show_debug_gui();
    }
};

GEAROENIX_START(GameApp);
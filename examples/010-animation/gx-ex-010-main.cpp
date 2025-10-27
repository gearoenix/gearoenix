#include <gearoenix/core/ecs/gx-cr-ecs-entity.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-utilities.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/animation/gx-phs-anm-animation.hpp>
#include <gearoenix/physics/animation/gx-phs-anm-armature.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gltf/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

template <typename T>
using GxEndCaller = gearoenix::core::job::EndCaller<T>;

using GxCoreApp = gearoenix::core::Application;
using GxEntity = gearoenix::core::ecs::Entity;
using GxEntityPtr = gearoenix::core::ecs::EntityPtr;
using GxEntityEndCaller = gearoenix::core::job::EndCaller<GxEntityPtr>;
using GxConstraintManager = gearoenix::physics::constraint::Manager;
using GxTransform = gearoenix::physics::Transformation;
using GxAnimPlayer = gearoenix::physics::animation::AnimationPlayer;
using GxArm = gearoenix::physics::animation::Armature;
using GxPath = gearoenix::platform::stream::Path;
using GxCamera = gearoenix::render::camera::Camera;
using GxCameraManager = gearoenix::render::camera::Manager;
using GxReflectionManager = gearoenix::render::reflection::Manager;
using GxSceneManager = gearoenix::render::scene::Manager;
using GxSkyboxManager = gearoenix::render::skybox::Manager;

struct GameApp final : GxCoreApp {
private:
    GxEntityPtr scene_entity;

public:
    GameApp()
    {
        gearoenix::render::gltf::load(
            GxPath::create_absolute("../../../../submodules/glTF-Sample-Assets/Models/RiggedSimple/glTF/RiggedSimple.gltf"),
            GxEndCaller<std::vector<GxEntityPtr>>([this](auto&& entities) {
                scene_entity = std::move(entities[0]);
                gltf_is_ready();
            }));
    }

    void gltf_is_ready()
    {
        const GxEndCaller<void> end([this] { scene_entity->add_to_world(); });

        GxSkyboxManager::get().build("sky", scene_entity.get(), GxPath::create_asset("sky.gx-cube-texture"), GxEntityEndCaller([end](auto&&) { (void)end; }));

        GxReflectionManager::get().build_baked("reflection", scene_entity.get(), GxPath::create_asset("exported.gx-reflection"), GxEntityEndCaller([end](auto&&) { (void)end; }));

        auto camera_entities = gearoenix::core::ecs::find<GxCamera>(scene_entity);
        const GxEntity* camera_entity = nullptr;
        for (const auto& entity : camera_entities | std::views::values) {
            if (GxCamera::Usage::Main == entity->get_component<GxCamera>()->get_usage()) {
                camera_entity = entity.get();
                break;
            }
        }
        if (camera_entity) {
            camera_is_ready(camera_entity, end);
        } else {
            GxCameraManager::get().build("camera", scene_entity.get(), GxEntityEndCaller([this, end](auto&& entity) {
                camera_is_ready(entity.get(), end);
            }));
        }
    }

    void camera_is_ready(const GxEntity* const entity, const GxEndCaller<void>&)
    {
        auto trn = entity->get_component_shared_ptr<GxTransform>();
        trn->local_look_at({ 19.0, 19.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 });
        (void)GxConstraintManager::get().create_jet_controller(entity->get_object_name() + "-controller", std::move(trn), scene_entity.get());
    }
};

GEAROENIX_START(GameApp);
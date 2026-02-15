#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

template <typename T>
using GxEndCaller = gearoenix::core::job::EndCaller<T>;

typedef gearoenix::core::Application GxCoreApp;
typedef gearoenix::core::ecs::EntityPtr GxEntityPtr;
typedef gearoenix::physics::constraint::Manager GxConstraintManager;
typedef gearoenix::physics::Transformation GxTransform;
typedef gearoenix::platform::stream::Path GxPath;
typedef gearoenix::render::camera::Manager GxCameraManager;
typedef gearoenix::render::scene::Manager GxSceneManager;
typedef gearoenix::render::skybox::Manager GxSkyboxManager;
typedef GxEndCaller<GxEntityPtr> GxEntityEndCaller;

struct GameApp final : GxCoreApp {
private:
    GxEntityPtr scene_entity;

public:
    GameApp()
        : scene_entity(GxSceneManager::get().build("scene", 0.0))
    {
        GxEndCaller<void> end([this] { scene_entity->add_to_world(); });

        GxSkyboxManager::get().build("skybox", scene_entity.get(), GxPath::create_asset("sky.hdr"), GxEntityEndCaller([end](auto&&) { (void)end; }));

        GxCameraManager::get().build("camera", scene_entity.get(), GxEntityEndCaller([this, end](auto&& entity) {
            auto trn = entity->template get_component_shared_ptr<GxTransform>();
            (void)GxConstraintManager::get().create_jet_controller(entity->get_object_name() + "-controller", std::move(trn), scene_entity.get());
            (void)end;
        }));

        GX_LOG_D("Initialised");
    }

    void update() override { Application::update(); }
};

GEAROENIX_START(GameApp);

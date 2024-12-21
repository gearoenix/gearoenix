#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/animation/gx-phs-anm-animation.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gltf/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

template <typename T>
using GxEndCallerShared = gearoenix::core::job::EndCallerShared<T>;
template <typename T>
using GxEndCallerT = gearoenix::core::job::EndCaller<T>;
using GxEndCaller = gearoenix::core::job::EndCaller<>;

using GxComp = gearoenix::core::ecs::Component;
using GxCoreApp = gearoenix::core::Application;
using GxPltApp = gearoenix::platform::Application;
using GxTransform = gearoenix::physics::Transformation;

using GxScene = gearoenix::render::scene::Scene;
using GxSceneBuilder = gearoenix::render::scene::Builder;
using GxSceneBuilderPtr = std::shared_ptr<GxSceneBuilder>;

using GxCameraBuilder = gearoenix::render::camera::Builder;
using GxCameraBuilderPtr = std::shared_ptr<GxCameraBuilder>;
using GxCameraBuilderEndCaller = GxEndCallerShared<GxCameraBuilder>;

using GxLightBuilder = gearoenix::render::light::Builder;
using GxLightBuilderPtr = std::shared_ptr<GxLightBuilder>;
using GxLightBuilderEndCaller = GxEndCallerShared<GxLightBuilder>;

using GxMesh = gearoenix::render::mesh::Mesh;
using GxMeshPtr = std::shared_ptr<GxMesh>;
using GxMeshEndCaller = GxEndCallerShared<GxMesh>;

using GxSkyboxBuilder = gearoenix::render::skybox::Builder;
using GxSkyboxBuilderPtr = std::shared_ptr<GxSkyboxBuilder>;
using GxSkyboxBuilderEndCaller = GxEndCallerShared<GxSkyboxBuilder>;

using GxReflectionBuilder = gearoenix::render::reflection::Builder;
using GxReflectionBuilderPtr = std::shared_ptr<GxReflectionBuilder>;
using GxReflectionBuilderEndCaller = GxEndCallerShared<GxReflectionBuilder>;

using GxPath = gearoenix::platform::stream::Path;

using GxAnimationPlayer = gearoenix::physics::animation::AnimationPlayer;

struct GameApp final : GxCoreApp {
    explicit GameApp(GxPltApp& plt_app);
    void update() override;
    void gltf_is_ready(const GxSceneBuilderPtr& scene_builder);
};

GameApp::GameApp(GxPltApp& plt_app)
    : GxCoreApp(plt_app)
{
    gearoenix::render::gltf::load(
        GxPath::create_asset("example-011.glb"),
        GxEndCallerT<std::vector<GxSceneBuilderPtr>>([this](auto&& scene_builders) {
            gltf_is_ready(scene_builders[0]);
        }),
        GxEndCaller([] {}));
}

void GameApp::update()
{
    GxCoreApp::update();
}

void GameApp::gltf_is_ready(const GxSceneBuilderPtr& scene_builder)
{
    if (const auto search = scene_builder->get_model_builders().find("Cesium_Man"); scene_builder->get_model_builders().end() != search) {
        auto* const player = search->second->get_entity_builder()->get_builder().get_component<GxAnimationPlayer>();
        player->set_loop_range_time(1.0e-10, 1.98);
    }

    render_engine.get_skybox_manager()->build(
        "hello-skybox",
        GxPath::create_asset("sky.gx-cube-texture"),
        GxEndCaller([] {}),
        GxSkyboxBuilderEndCaller([scene_builder](GxSkyboxBuilderPtr&& skybox_builder) {
            scene_builder->add(std::move(skybox_builder));
        }));

    render_engine.get_reflection_manager()->build_baked(
        "baked-reflection", nullptr,
        GxPath::create_asset("exported.gx-reflection"),
        GxReflectionBuilderEndCaller([scene_builder](GxReflectionBuilderPtr&& baked_reflection_probe_builder) {
            scene_builder->add(std::move(baked_reflection_probe_builder));
        }),
        GxEndCaller([] {}));

    scene_builder->get_scene().set_enabled(true);

    auto& camera_builder = *scene_builder->get_camera_builders().begin()->second;
    auto trn = std::dynamic_pointer_cast<GxTransform>(camera_builder.get_transformation().get_component_self().lock());
    trn->local_look_at({ -19.0, -19.0, 5.0 }, { -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
    const auto& cm = *render_engine.get_physics_engine()->get_constraint_manager();
    auto ctrl_name = camera_builder.get_entity_builder()->get_builder().get_name() + "-controller";
    (void)cm.create_jet_controller(std::move(ctrl_name), std::move(trn), GxEndCaller([] { }));
}

GEAROENIX_START(GameApp)
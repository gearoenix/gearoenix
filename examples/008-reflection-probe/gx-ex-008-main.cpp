#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-jet-controller.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-builder.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-runtime.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-cube.hpp>

#define GX_EXAMPLE_008_EXPORT_ENVIRONMENT
#define GX_EXAMPLE_008_EXPORT_REFLECTION

template <typename T>
using GxEndCallerShared = gearoenix::core::job::EndCallerShared<T>;
using GxEndCaller = gearoenix::core::job::EndCaller<>;

using GxComp = gearoenix::core::ecs::Component;
using GxCoreApp = gearoenix::core::Application;
using GxPltApp = gearoenix::platform::Application;
using GxTransformComp = gearoenix::physics::TransformationComponent;
using GxTransform = gearoenix::physics::Transformation;
using GxJetCtrl = gearoenix::physics::constraint::JetController;

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

using GxPbr = gearoenix::render::material::Pbr;
using GxPbrPtr = std::shared_ptr<GxPbr>;
using GxPbrEndCaller = GxEndCallerShared<GxPbr>;

using GxSkyboxBuilder = gearoenix::render::skybox::Builder;
using GxSkyboxBuilderPtr = std::shared_ptr<GxSkyboxBuilder>;
using GxSkyboxBuilderEndCaller = GxEndCallerShared<GxSkyboxBuilder>;

using GxReflectionBuilder = gearoenix::render::reflection::Builder;
using GxReflectionBuilderPtr = std::shared_ptr<GxReflectionBuilder>;
using GxReflectionBuilderEndCaller = GxEndCallerShared<GxReflectionBuilder>;
using GxReflectionRuntime = gearoenix::render::reflection::Runtime;

using GxPath = gearoenix::platform::stream::Path;
using GxStream = gearoenix::platform::stream::Stream;
using GxLocal = gearoenix::platform::stream::Local;

using GxAabb3 = gearoenix::math::Aabb3<double>;
using GxVec3 = gearoenix::math::Vec3<double>;

struct GameApp final : GxCoreApp {
    explicit GameApp(GxPltApp& plt_app)
        : Application(plt_app)
    {
        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, GxEndCaller([] { }));
        scene_builder->get_scene().set_enabled(true);

        for (int metallic_i = 0; metallic_i < 10; ++metallic_i) {
            for (int roughness_i = 0; roughness_i < 10; ++roughness_i) {
                const auto metallic = 0.05f + static_cast<float>(metallic_i) * 0.1f;
                const auto roughness = 0.05f + static_cast<float>(roughness_i) * 0.1f;
                const auto postfix = "-metallic:" + std::to_string(metallic) + "-roughness:" + std::to_string(roughness);
                render_engine.get_material_manager()->get_pbr(
                    "material-" + postfix,
                    GxPbrEndCaller(
                        [this, metallic, roughness, p = postfix, sb = scene_builder](GxPbrPtr&& material) mutable {
                            material_is_ready(std::move(material), metallic, roughness, std::move(p), std::move(sb));
                        }));
            }
        }

        render_engine.get_skybox_manager()->build(
            "hello-skybox",
            GxPath::create_asset("sky.hdr"),
            GxEndCaller([] {}),
            GxSkyboxBuilderEndCaller([scene_builder](GxSkyboxBuilderPtr&& skybox_builder) {
                scene_builder->add(std::move(skybox_builder));
            }));

        render_engine.get_camera_manager()->build(
            "camera", nullptr,
            GxCameraBuilderEndCaller([this, scene_builder](GxCameraBuilderPtr&& camera_builder) {
                auto trn = std::dynamic_pointer_cast<GxTransform>(camera_builder->get_transformation().get_component_self().lock());
                trn->local_translate({ -19.0, -19.0, 5.0 });
                trn->local_look_at({ -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
                const auto& cm = *render_engine.get_physics_engine()->get_constraint_manager();
                auto ctrl_name = camera_builder->get_entity_builder()->get_builder().get_name() + "-controller";
                (void)cm.create_jet_controller(std::move(ctrl_name), std::move(trn), camera_builder->get_id(), GxEndCaller([] { }));
                scene_builder->add(std::move(camera_builder));
            }),
            GxEndCaller([] {}));

        render_engine.get_reflection_manager()->build_runtime(
            "hello-runtime-reflection-probe", nullptr,
            GxAabb3(GxVec3(100.0), GxVec3(-100.0)),
            GxAabb3(GxVec3(20.0), GxVec3(-20.0)),
            GxAabb3(GxVec3(100.0), GxVec3(-100.0)),
            1024, 256, 512,
            GxEndCaller([] {}),
            GxReflectionBuilderEndCaller([this, scene_builder](GxReflectionBuilderPtr&& runtime_reflection_probe_builder) {
#if defined(GX_EXAMPLE_008_EXPORT_ENVIRONMENT) || defined(GX_EXAMPLE_008_EXPORT_REFLECTION)
                const auto id = runtime_reflection_probe_builder->get_id();
                runtime_reflection_probe_builder->get_runtime().set_on_rendered([id, this] {
                    const auto* const r = render_engine.get_world()->get_component<GxReflectionRuntime>(id);
#if defined(GX_EXAMPLE_008_EXPORT_REFLECTION)
                    const std::shared_ptr<GxStream> rl(
                        new GxLocal(platform_application, "exported.gx-reflection", true));
                    r->export_baked(rl, GxEndCaller([] { }));
#endif
#if defined(GX_EXAMPLE_008_EXPORT_ENVIRONMENT)
                    const std::shared_ptr<GxStream> tl(
                        new GxLocal(platform_application, "sky.gx-cube-texture", true));
                    r->get_environment()->write(tl, GxEndCaller([] { }));
#endif
                });
#endif
                scene_builder->add(std::move(runtime_reflection_probe_builder));
            }));

        GX_LOG_D("Initialised");
    }

    void material_is_ready(
        GxPbrPtr&& material, const float metallic, const float roughness, std::string&& postfix, GxSceneBuilderPtr&& scene_builder)
    {
        material->get_normal_metallic_factor().w = metallic;
        material->get_emission_roughness_factor().w = roughness;

        render_engine.get_mesh_manager()->build_icosphere(
            4,
            std::move(material),
            GxMeshEndCaller([this, metallic, roughness, p = std::move(postfix), sb = std::move(scene_builder)](GxMeshPtr&& m) mutable {
                mesh_is_ready(std::move(m), metallic, roughness, std::move(p), std::move(sb));
            }));
    }

    void mesh_is_ready(GxMeshPtr&& mesh, const float metallic, const float roughness, std::string&& postfix, GxSceneBuilderPtr&& scene_builder)
    {
        auto model_builder = render_engine.get_model_manager()->build(
            "icosphere" + postfix, nullptr,
            { std::move(mesh) },
            GxEndCaller([] { }),
            true);
        model_builder->get_transformation().local_translate(
            { static_cast<double>(metallic) * 30.0 - 15.0,
                static_cast<double>(roughness) * 30.0 - 15.0,
                0.0 });
        scene_builder->add(std::move(model_builder));
    }

    void update() override
    {
        Application::update();
    }
};

GEAROENIX_START(GameApp)

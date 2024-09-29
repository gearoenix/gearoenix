#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-builder.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-runtime.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

template <typename T>
using GxEndCallerShared = gearoenix::core::job::EndCallerShared<T>;
using GxEndCaller = gearoenix::core::job::EndCaller<>;

using GxComp = gearoenix::core::ecs::Component;
using GxCoreApp = gearoenix::core::Application;
using GxPltApp = gearoenix::platform::Application;
using GxTransformComp = gearoenix::physics::TransformationComponent;

using GxScene = gearoenix::render::scene::Scene;
using GxSceneBuilder = gearoenix::render::scene::Builder;
using GxSceneBuilderPtr = std::shared_ptr<GxSceneBuilder>;

using GxCameraBuilder = gearoenix::render::camera::Builder;
using GxCameraBuilderPtr = std::shared_ptr<GxCameraBuilder>;
using GxCameraBuilderEndCaller = GxEndCallerShared<GxCameraBuilder>;
using GxJetCtrl = gearoenix::render::camera::JetController;

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

struct GameApp final : public GxCoreApp {
    std::unique_ptr<GxJetCtrl> camera_controller;

    explicit GameApp(GxPltApp& plt_app)
        : Application(plt_app)
    {
        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, GxEndCaller([] {}));
        scene_builder->get_scene().set_enabled(true);

        for (std::size_t metallic_i = 0; metallic_i < 10; ++metallic_i) {
            for (std::size_t roughness_i = 0; roughness_i < 10; ++roughness_i) {
                const auto metallic = 0.05f + static_cast<float>(metallic_i) * 0.1f;
                const auto roughness = 0.05f + static_cast<float>(roughness_i) * 0.1f;
                const auto postfix = "metallic: " + std::to_string(metallic) + ", roughness: " + std::to_string(roughness);
                render_engine.get_material_manager()->get_pbr(
                    "material" + postfix,
                    GxPbrEndCaller([this, metallic, p = postfix, roughness, sb = scene_builder](GxPbrPtr&& material) mutable {
                        icosphere_material_is_ready(std::move(material), std::move(p), metallic, roughness, std::move(sb));
                    }));
            }
        }

        render_engine.get_material_manager()->get_pbr(
            "ground",
            GxPbrEndCaller([this, sb = scene_builder](GxPbrPtr&& material) mutable {
                plate_material_is_ready(std::move(material), std::move(sb));
            }));

        render_engine.get_camera_manager()->build(
            "camera", nullptr,
            GxCameraBuilderEndCaller([this, scene_builder](GxCameraBuilderPtr&& camera_builder) {
                camera_builder->get_transformation().local_look_at(
                    { -19.0, -19.0, 5.0 }, { -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
                camera_controller = std::make_unique<GxJetCtrl>(
                    render_engine,
                    camera_builder->get_entity_builder()->get_builder().get_id());
                camera_controller->set_movement_speed(10.0);
                scene_builder->add(std::move(camera_builder));
            }),
            GxEndCaller([] {}));

        for (int light_index = 0; light_index < 4; ++light_index) {
            render_engine.get_light_manager()->build_shadow_caster_directional(
                "directional-light-" + std::to_string(light_index), nullptr, 1024, 50.0f, 1.0f, 35.0f,
                GxLightBuilderEndCaller([scene_builder, light_index](GxLightBuilderPtr&& light_builder) {
                    light_builder->get_shadow_caster_directional()->get_shadow_transform()->local_look_at(
                        { static_cast<double>(((light_index & 1) << 1) - 1) * 11.0,
                            static_cast<double>(((light_index >> 1) << 1) - 1) * 11.0,
                            11.0 },
                        { 0.0, 0.0, 0.0 },
                        { 0.0, 1.0, 0.0 });
                    light_builder->get_light().colour = { 2.0f, 2.0f, 2.0f };
                    scene_builder->add(std::move(light_builder));
                }),
                GxEndCaller([] {}));
        }

        GX_LOG_D("Initialised");
    }

    void icosphere_mesh_is_ready(
        GxMeshPtr&& mesh, const std::string& postfix, const GxSceneBuilderPtr& scene_builder,
        const float metallic, const float roughness)
    {
        auto model_builder = render_engine.get_model_manager()->build(
            "icosphere" + postfix, nullptr,
            { std::move(mesh) },
            GxEndCaller([] {}),
            true);
        model_builder->get_transformation().local_translate({ static_cast<double>(metallic) * 30.0 - 15.0,
            static_cast<double>(roughness) * 30.0 - 15.0,
            0.0 });
        scene_builder->add(std::move(model_builder));
    }

    void icosphere_material_is_ready(
        GxPbrPtr&& material, std::string&& postfix, const float metallic, const float roughness,
        GxSceneBuilderPtr&& scene_builder)
    {
        material->get_albedo_factor().x = 0.999f;
        material->get_albedo_factor().y = 0.1f;
        material->get_albedo_factor().z = 0.4f;
        material->get_normal_metallic_factor().w = metallic;
        material->get_emission_roughness_factor().w = roughness;

        render_engine.get_mesh_manager()->build_icosphere(
            4,
            std::move(material),
            GxMeshEndCaller([this, sb = std::move(scene_builder), metallic, roughness, p = std::move(postfix)](GxMeshPtr&& mesh) {
                icosphere_mesh_is_ready(std::move(mesh), p, sb, metallic, roughness);
            }));
    }

    void plate_mesh_is_ready(GxMeshPtr&& mesh, const GxSceneBuilderPtr& scene_builder)
    {
        auto model_builder = render_engine.get_model_manager()->build(
            "ground", nullptr,
            { std::move(mesh) },
            GxEndCaller([] {}),
            true);
        model_builder->get_transformation().local_scale(30.0);
        model_builder->get_transformation().local_translate({ 0.0, 0.0, -5.0 });
        scene_builder->add(std::move(model_builder));
    }

    void plate_material_is_ready(GxPbrPtr&& material, GxSceneBuilderPtr&& scene_builder)
    {
        material->get_albedo_factor() = { 0.1f, 0.999f, 0.3f, 1.0f };
        material->get_normal_metallic_factor().w = 0.001;
        material->get_emission_roughness_factor().w = 0.99;

        render_engine.get_mesh_manager()->build_plate(
            std::move(material),
            GxMeshEndCaller([this, sb = std::move(scene_builder)](GxMeshPtr&& mesh) {
                plate_mesh_is_ready(std::move(mesh), sb);
            }));
    }

    void update() override
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)

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

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        render_engine.get_mesh_manager()->build_icosphere(4,
            gearoenix::core::job::EndCallerShared<gearoenix::render::mesh::Mesh>([this](std::shared_ptr<gearoenix::render::mesh::Mesh>&& m) {
                mesh_is_ready(m);
            }));
    }

    void mesh_is_ready(const std::shared_ptr<gearoenix::render::mesh::Mesh>& icosphere_mesh)
    {
        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, gearoenix::core::job::EndCaller([] {}));
        scene_builder->get_scene().set_enabled(true);

        for (int metallic_i = 0; metallic_i < 10; ++metallic_i) {
            for (int roughness_i = 0; roughness_i < 10; ++roughness_i) {
                const auto metallic = 0.05f + static_cast<float>(metallic_i) * 0.1f;
                const auto roughness = 0.05f + static_cast<float>(roughness_i) * 0.1f;
                const auto postfix = "-metallic:" + std::to_string(metallic) + "-roughness:" + std::to_string(roughness);
                render_engine.get_material_manager()->get_pbr(
                    "material" + postfix,
                    gearoenix::core::job::EndCallerShared<gearoenix::render::material::Pbr>(
                        [this, metallic, roughness, postfix, icosphere_mesh, scene_builder](std::shared_ptr<gearoenix::render::material::Pbr>&& material) noexcept {
                            material->get_normal_metallic_factor().w = metallic;
                            material->get_emission_roughness_factor().w = roughness;
                            auto model_builder = render_engine.get_model_manager()->build(
                                "icosphere" + postfix,
                                std::shared_ptr(icosphere_mesh),
                                std::move(material),
                                gearoenix::core::job::EndCaller([] {}),
                                true);
                            model_builder->get_transformation().local_translate(
                                { static_cast<double>(metallic) * 30.0 - 15.0,
                                    static_cast<double>(roughness) * 30.0 - 15.0,
                                    0.0 });
                            scene_builder->add(std::move(model_builder));
                        }));
            }
        }

        render_engine.get_skybox_manager()->build(
            "hello-skybox",
            gearoenix::platform::stream::Path::create_asset("sky.hdr"),
            gearoenix::core::job::EndCaller([] {}),
            gearoenix::core::job::EndCallerShared<gearoenix::render::skybox::Builder>([scene_builder](std::shared_ptr<gearoenix::render::skybox::Builder>&& skybox_builder) {
                scene_builder->add(std::move(skybox_builder));
            }));

        render_engine.get_camera_manager()->build(
            "camera",
            gearoenix::core::job::EndCallerShared<gearoenix::render::camera::Builder>([this, scene_builder](std::shared_ptr<gearoenix::render::camera::Builder>&& camera_builder) {
                auto& camera_transform = camera_builder->get_transformation();
                camera_transform.local_translate({ -19.0, -19.0, 5.0 });
                camera_transform.local_look_at({ -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
                camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
                    render_engine,
                    camera_builder->get_entity_builder()->get_builder().get_id());
                scene_builder->add(std::move(camera_builder));
            }),
            gearoenix::core::job::EndCaller([] {}));

        render_engine.get_reflection_manager()->build_runtime(
            "hello-runtime-reflection-probe",
            gearoenix::math::Aabb3(gearoenix::math::Vec3(100.0), gearoenix::math::Vec3(-100.0)),
            gearoenix::math::Aabb3(gearoenix::math::Vec3(20.0), gearoenix::math::Vec3(-20.0)),
            gearoenix::math::Aabb3(gearoenix::math::Vec3(100.0), gearoenix::math::Vec3(-100.0)),
            1024, 256, 512,
            gearoenix::core::job::EndCaller([] {}),
            gearoenix::core::job::EndCallerShared<gearoenix::render::reflection::Builder>([this, scene_builder = std::move(scene_builder)](std::shared_ptr<gearoenix::render::reflection::Builder>&& runtime_reflection_probe_builder) {
#if defined(GX_EXAMPLE_008_EXPORT_ENVIRONMENT) || defined(GX_EXAMPLE_008_EXPORT_REFLECTION)
                const auto id = runtime_reflection_probe_builder->get_id();
                runtime_reflection_probe_builder->get_runtime().set_on_rendered([id, this] {
                    const auto* const r = render_engine.get_world()->get_component<gearoenix::render::reflection::Runtime>(id);
#if defined(GX_EXAMPLE_008_EXPORT_REFLECTION)
                    const std::shared_ptr<gearoenix::platform::stream::Stream> rl(
                        new gearoenix::platform::stream::Local(platform_application, "exported.gx-reflection", true));
                    r->export_baked(rl, gearoenix::core::job::EndCaller([] {}));
#endif
#if defined(GX_EXAMPLE_008_EXPORT_ENVIRONMENT)
                    const std::shared_ptr<gearoenix::platform::stream::Stream> tl(
                        new gearoenix::platform::stream::Local(platform_application, "sky.gx-cube-texture", true));
                    r->get_environment()->write(tl, gearoenix::core::job::EndCaller([] {}));
#endif
                });
#endif
                scene_builder->add(std::move(runtime_reflection_probe_builder));
            }));

        GX_LOG_D("Initialised");
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-builder.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-runtime.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-cube.hpp>

//#define GX_EXAMPLE_008_EXPORT_ENVIRONMENT
#define GX_EXAMPLE_008_EXPORT_REFLECTION

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        const auto scene_builder = render_engine.get_scene_manager()->build("scene");

        gearoenix::core::sync::EndCallerIgnored end_callback([scene_builder] {});

        auto icosphere_mesh = render_engine.get_mesh_manager()->build_icosphere(
            4,
            gearoenix::core::sync::EndCallerIgnored(end_callback));

        gearoenix::render::material::Pbr material(render_engine);
        material.get_albedo_factor().x = 0.999f;
        material.get_albedo_factor().y = 0.1f;
        material.get_albedo_factor().z = 0.4f;
        for (std::size_t metallic_i = 0; metallic_i < 10; ++metallic_i) {
            material.get_normal_metallic_factor().w = static_cast<float>(metallic_i) * 0.1f;
            for (std::size_t roughness_i = 0; roughness_i < 10; ++roughness_i) {
                material.get_emission_roughness_factor().w = static_cast<float>(roughness_i) * 0.1f;
                auto model_builder = render_engine.get_model_manager()->build(
                    "icosphere-" + std::to_string(metallic_i) + "-" + std::to_string(roughness_i),
                    std::shared_ptr(icosphere_mesh),
                    gearoenix::core::sync::EndCallerIgnored(end_callback),
                    true);
                model_builder->set_material(material);
                model_builder->get_transformation().translate({ static_cast<double>(metallic_i) * 3.0 - 15.0,
                    static_cast<double>(roughness_i) * 3.0 - 15.0,
                    0.0 });
                scene_builder->add(std::move(model_builder));
            }
        }

        auto skybox_builder = render_engine.get_skybox_manager()->build(
            "hello-skybox",
            gearoenix::platform::stream::Path::create_asset("sky.hdr"),
            end_callback);
        scene_builder->add(std::move(skybox_builder));

        auto camera_builder = render_engine.get_camera_manager()->build("camera");
        auto& camera_transform = camera_builder->get_transformation();
        camera_transform.translate({ -19.0, -19.0, 5.0 });
        camera_transform.look_at({ -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
        camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
            render_engine,
            camera_builder->get_entity_builder()->get_builder().get_id());
        scene_builder->add(std::move(camera_builder));

        auto runtime_reflection_probe_builder = render_engine.get_reflection_manager()->build_runtime(
            "hello-runtime-reflection-probe",
            gearoenix::math::Aabb3(gearoenix::math::Vec3(100.0), gearoenix::math::Vec3(-100.0)),
            gearoenix::math::Aabb3(gearoenix::math::Vec3(20.0), gearoenix::math::Vec3(-20.0)),
            gearoenix::math::Aabb3(gearoenix::math::Vec3(100.0), gearoenix::math::Vec3(-100.0)),
            1024, 128, 256,
            end_callback);
#if defined(GX_EXAMPLE_008_EXPORT_ENVIRONMENT) || defined(GX_EXAMPLE_008_EXPORT_REFLECTION)
        const auto id = runtime_reflection_probe_builder->get_entity_builder()->get_builder().get_id();
        runtime_reflection_probe_builder->get_runtime().set_on_rendered([id, this] {
            const auto* const r = render_engine.get_world()->get_component<gearoenix::render::reflection::Runtime>(id);
#if defined(GX_EXAMPLE_008_EXPORT_REFLECTION)
            std::shared_ptr<gearoenix::platform::stream::Stream> rl(new gearoenix::platform::stream::Local(platform_application, "exported.gx-reflection", true));
            r->export_baked(rl);
#endif
#if defined(GX_EXAMPLE_008_EXPORT_ENVIRONMENT)
            std::shared_ptr<gearoenix::platform::stream::Stream> tl(new gearoenix::platform::stream::Local(platform_application, "sky.gx-cube-texture", true));
            r->get_environment()->write(tl, GX_DEFAULT_IGNORED_END_CALLER);
#endif
        });
#endif
        scene_builder->add(std::move(runtime_reflection_probe_builder));

        GX_LOG_D("Initialised");
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)

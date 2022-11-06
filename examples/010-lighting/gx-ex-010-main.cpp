#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
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

        auto plate_mesh = render_engine.get_mesh_manager()->build_plate(
            gearoenix::core::sync::EndCallerIgnored(end_callback));

        for (std::size_t metallic_i = 0; metallic_i < 10; ++metallic_i) {
            for (std::size_t roughness_i = 0; roughness_i < 10; ++roughness_i) {
                auto material = render_engine.get_material_manager()->get_pbr(
                        "material-" + std::to_string(metallic_i) + "-" + std::to_string(roughness_i), end_callback);
                material->get_albedo_factor().x = 0.999f;
                material->get_albedo_factor().y = 0.1f;
                material->get_albedo_factor().z = 0.4f;
                material->get_normal_metallic_factor().w = static_cast<float>(metallic_i) * 0.1f;
                material->get_emission_roughness_factor().w = static_cast<float>(roughness_i) * 0.1f;
                auto model_builder = render_engine.get_model_manager()->build(
                    "icosphere-" + std::to_string(metallic_i) + "-" + std::to_string(roughness_i),
                    std::shared_ptr(icosphere_mesh),
                    std::move(material),
                    gearoenix::core::sync::EndCallerIgnored(end_callback),
                    true);
                model_builder->get_transformation().local_translate({ static_cast<double>(metallic_i) * 3.0 - 15.0,
                    static_cast<double>(roughness_i) * 3.0 - 15.0,
                    0.0 });
                scene_builder->add(std::move(model_builder));
            }
        }
        auto material = render_engine.get_material_manager()->get_pbr("ground", end_callback);
        material->get_albedo_factor() = { 0.1f, 0.999f, 0.3f, 1.0f };
        material->get_normal_metallic_factor().w = 0.001;
        material->get_emission_roughness_factor().w = 0.99;
        auto ground_model_builder = render_engine.get_model_manager()->build(
            "ground",
            std::shared_ptr(plate_mesh),
            std::move(material),
            gearoenix::core::sync::EndCallerIgnored(end_callback),
            true);
        ground_model_builder->get_transformation().local_scale(30.0);
        ground_model_builder->get_transformation().local_translate({ 0.0, 0.0, -5.0 });
        scene_builder->add(std::move(ground_model_builder));

        auto camera_builder = render_engine.get_camera_manager()->build("camera");
        camera_builder->get_transformation().local_look_at({ -19.0, -19.0, 5.0 }, { -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
        camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
            render_engine,
            camera_builder->get_entity_builder()->get_builder().get_id());
        scene_builder->add(std::move(camera_builder));

        auto light_builder_0 = render_engine.get_light_manager()->build_shadow_caster_directional(
            "directional-light-0",
            1024,
            10.0f,
            1.0f,
            35.0f,
            end_callback);
        light_builder_0->get_transformation().local_look_at({ 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
        light_builder_0->get_light().colour = { 2.0f, 2.0f, 2.0f };
        scene_builder->add(std::move(light_builder_0));

        GX_LOG_D("Initialised");
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)

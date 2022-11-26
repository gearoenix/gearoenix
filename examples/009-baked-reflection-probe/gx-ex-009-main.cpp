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

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        gearoenix::core::sync::EndCaller end_callback([] {});

        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, gearoenix::core::sync::EndCaller(end_callback));
        scene_builder->get_scene().enabled = true;

        auto icosphere_mesh = render_engine.get_mesh_manager()->build_icosphere(
            4,
            gearoenix::core::sync::EndCaller(end_callback));

        for (std::size_t metallic_i = 0; metallic_i < 10; ++metallic_i) {
            for (std::size_t roughness_i = 0; roughness_i < 10; ++roughness_i) {
                auto material = render_engine.get_material_manager()->get_pbr(
                    "metallic: " + std::to_string(metallic_i) + ", roughness: " + std::to_string(roughness_i), end_callback);
                material->get_albedo_factor().x = 0.999f;
                material->get_albedo_factor().y = 0.1f;
                material->get_albedo_factor().z = 0.4f;
                material->get_normal_metallic_factor().w = static_cast<float>(metallic_i) * 0.1f;
                material->get_emission_roughness_factor().w = static_cast<float>(roughness_i) * 0.1f;
                auto model_builder = render_engine.get_model_manager()->build(
                    "icosphere-" + std::to_string(metallic_i) + "-" + std::to_string(roughness_i),
                    std::shared_ptr(icosphere_mesh),
                    std::move(material),
                    gearoenix::core::sync::EndCaller(end_callback),
                    true);
                model_builder->get_transformation().local_translate({ static_cast<double>(metallic_i) * 3.0 - 15.0,
                    static_cast<double>(roughness_i) * 3.0 - 15.0,
                    0.0 });
                scene_builder->add(std::move(model_builder));
            }
        }

        auto skybox_builder = render_engine.get_skybox_manager()->build(
            "hello-skybox",
            gearoenix::platform::stream::Path::create_asset("sky.gx-cube-texture"),
            end_callback);
        scene_builder->add(std::move(skybox_builder));

        auto camera_builder = render_engine.get_camera_manager()->build(
            "camera", gearoenix::core::sync::EndCaller(end_callback));
        auto& camera_transform = camera_builder->get_transformation();
        camera_transform.local_translate({ -19.0, -19.0, 5.0 });
        camera_transform.local_look_at({ -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
        camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
            render_engine,
            camera_builder->get_entity_builder()->get_builder().get_id());
        scene_builder->add(std::move(camera_builder));

        auto baked_reflection_probe_builder = render_engine.get_reflection_manager()->build_baked(
            "baked-reflection",
            gearoenix::platform::stream::Path::create_asset("exported.gx-reflection"),
            end_callback);
        scene_builder->add(std::move(baked_reflection_probe_builder));

        GX_LOG_D("Initialised");
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)

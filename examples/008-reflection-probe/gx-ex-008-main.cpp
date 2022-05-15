#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

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

        auto model_builder = render_engine.get_model_manager()->build(
            "icosphere",
            std::move(icosphere_mesh),
            gearoenix::core::sync::EndCallerIgnored(end_callback),
            true);
        model_builder->set_material(gearoenix::render::material::Pbr(render_engine));
        scene_builder->add(std::move(model_builder));

        auto skybox_builder = render_engine.get_skybox_manager()->build(
            "hello-skybox",
            gearoenix::platform::stream::Path::create_asset("sky.hdr"),
            end_callback);
        scene_builder->add(std::move(skybox_builder));

        auto camera_builder = render_engine.get_camera_manager()->build("camera");
        camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
            render_engine,
            camera_builder->get_entity_builder()->get_builder().get_id());
        scene_builder->add(std::move(camera_builder));

        auto runtime_reflection_probe_builder = render_engine.get_reflection_manager()->build_runtime(
            "hello-runtime-reflection-probe",
            gearoenix::math::Aabb3(gearoenix::math::Vec3(10.0), gearoenix::math::Vec3(-10.0)),
            gearoenix::math::Aabb3(gearoenix::math::Vec3(1.0), gearoenix::math::Vec3(-1.0)),
            gearoenix::math::Aabb3(gearoenix::math::Vec3(10.0), gearoenix::math::Vec3(-10.0)),
            512, 128, 256,
            end_callback);
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

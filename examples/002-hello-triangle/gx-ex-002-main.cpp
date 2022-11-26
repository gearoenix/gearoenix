#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

struct GameApp final : public gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        std::vector<gearoenix::render::PbrVertex> vertices(3);
        vertices[0].position = { 1.0f, -1.0f, 0.0f };
        vertices[0].normal = { 0.0f, 0.0f, 1.0f };
        vertices[0].tangent = { 0.0f, 1.0f, 0.0f, 1.0f };
        vertices[0].uv = { 1.0f, -1.0f };
        vertices[1].position = { 0.0f, 1.0f, 0.0f };
        vertices[1].normal = { 0.0f, 0.0f, 1.0f };
        vertices[1].tangent = { 0.0f, 1.0f, 0.0f, 1.0f };
        vertices[1].uv = { 0.0f, 1.0f };
        vertices[2].position = { -1.0f, -1.0f, 0.0f };
        vertices[2].normal = { 0.0f, 0.0f, 1.0f };
        vertices[2].tangent = { 0.0f, 1.0f, 0.0f, 1.0f };
        vertices[2].uv = { -1.0f, -1.0f };

        std::vector<std::uint32_t> indices = { 0, 1, 2 };

        gearoenix::core::sync::EndCaller end_callback([] {});

        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, gearoenix::core::sync::EndCaller(end_callback));
        scene_builder->get_scene().enabled = true;

        auto mesh = render_engine.get_mesh_manager()->build(
            "triangle-mesh",
            std::move(vertices),
            std::move(indices),
            end_callback);

        auto material = render_engine.get_material_manager()->get_pbr("material", end_callback);

        auto model_builder = render_engine.get_model_manager()->build(
            "triangle-model",
            std::move(mesh),
            std::move(material),
            gearoenix::core::sync::EndCaller(end_callback),
            true);

        auto camera_builder = render_engine.get_camera_manager()->build("camera", gearoenix::core::sync::EndCaller(end_callback));
        camera_builder->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });

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
        scene_builder->add(std::move(model_builder));
        scene_builder->add(std::move(camera_builder));
    }

    void update() noexcept final
    {
        Application::update();
    }
};

GEAROENIX_START(GameApp)